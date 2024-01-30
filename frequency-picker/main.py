import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel, QVBoxLayout, QPushButton, QFileDialog, QWidget, QLineEdit
from PyQt5.QtGui import QPixmap, QPainter, QPen, QColor
from PyQt5.QtCore import Qt, QRect


class ImageLabel(QLabel):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setMouseTracking(True)  # Enable mouse tracking for this label

        self.horizontal_line = QLabel(self)
        self.horizontal_line.setStyleSheet("background-color: #FF5722")
        self.vertical_line = QLabel(self)
        self.vertical_line.setStyleSheet("background-color: #FF5722")

        self.selected_positions = []
        self.selected_box = QRect()

    def main_window(self):
        return self.parent().parent()


    def paintEvent(self, event):
        super().paintEvent(event)
        painter = QPainter(self)
        painter.setPen(QPen(Qt.red, 2, Qt.SolidLine))
        if not self.selected_box.isNull():
            painter.drawRect(self.selected_box)

    def update_selected_box(self):
        if len(self.selected_positions) == 2:
            pos1, pos2 = self.selected_positions
            self.selected_box = QRect(min(pos1[0], pos2[0]), min(pos1[1], pos2[1]),
                                      abs(pos1[0] - pos2[0]), abs(pos1[1] - pos2[1]))
        else:
            self.selected_box = QRect()
        self.update()

    def mouseMoveEvent(self, event):
        if not self.pixmap():
            return
        x = event.pos().x()
        y = event.pos().y()

        pixmap_geometry = self.geometry()
   
        self.main_window().status_label.setText(f"Position: ({x}, {y})")

        self.horizontal_line.setGeometry(0, y, pixmap_geometry.width(), 1)
        self.vertical_line.setGeometry(x, 0, 1, pixmap_geometry.height())
        self.horizontal_line.show()
        self.vertical_line.show()
        self.horizontal_line.raise_()
        self.vertical_line.raise_()

    def hide_lines(self):
        self.horizontal_line.hide()
        self.vertical_line.hide()

    def mousePressEvent(self, event):
        x = event.pos().x()
        y = event.pos().y()
        self.selected_positions.append((x, y))
        if len(self.selected_positions) == 2:
            pos1, pos2 = self.selected_positions
            self.selected_box = QRect(min(pos1[0], pos2[0]), min(pos1[1], pos2[1]),
                                    abs(pos1[0] - pos2[0]), abs(pos1[1] - pos2[1]))
            self.update_selected_box()
            self.main_window().status_label.setText(f"Box: {self.selected_box.width()}x{self.selected_box.height()} at ({self.selected_box.x()}, {self.selected_box.y()})")
            self.main_window().calculate()
        
        if len(self.selected_positions) > 2:
            self.main_window().reset()
            self.main_window().status_label.setText(f"Reset.")
            self.selected_positions.append((x, y))

        


class ImageSelector(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Image Selector")
        self.setFixedSize(1900, 1000)

        self.image_label = ImageLabel(self)
        self.image_label.setAlignment(Qt.AlignCenter)
        self.image_label.setScaledContents(True)
        self.image_label.setMouseTracking(True)

        self.status_label = QLabel(self)
        self.status_label.setAlignment(Qt.AlignCenter)

        layout = QVBoxLayout()
        layout.addWidget(self.image_label)
        layout.addWidget(self.status_label)

        position_layout = QVBoxLayout()
        layout.addLayout(position_layout)

        self.frequency_shift_label = QLabel("Downsampling shift [Hz] - Insert into EXP266 config.", self)
        position_layout.addWidget(self.frequency_shift_label)

        self.frequency_shift = QLineEdit(self)
        position_layout.addWidget(self.frequency_shift)
        self.frequency_shift.setReadOnly(True)


        self.cutoff_frequency_label = QLabel("Downsampling cut-off frequency [Hz] - Insert into EXP266 config.", self)
        position_layout.addWidget(self.cutoff_frequency_label)

        self.cutoff_frequency = QLineEdit(self)
        position_layout.addWidget(self.cutoff_frequency)
        self.cutoff_frequency.setReadOnly(True)

        self.output_sampling_label = QLabel("Output sample rate [Hz] - Use this to read the samples from now on!", self)
        position_layout.addWidget(self.output_sampling_label)

        self.output_sampling = QLineEdit(self)
        position_layout.addWidget(self.output_sampling)
        self.output_sampling.setReadOnly(True)

        self.output_size_label = QLabel("Probable downsampling output size [MB]", self)
        position_layout.addWidget(self.output_size_label)

        self.output_size = QLineEdit(self)
        position_layout.addWidget(self.output_size)
        self.output_size.setReadOnly(True)


        self.sampling_rate_label = QLabel("Input Sampling Rate - SDR Setting", self)
        position_layout.addWidget(self.sampling_rate_label)

        self.sampling_rate = QLineEdit(self)
        self.sampling_rate.setText("1500000")
        position_layout.addWidget(self.sampling_rate)

        # self.num_samples_label = QLabel("Number of Samples", self)
        # position_layout.addWidget(self.num_samples_label)

        # self.num_samples = QLineEdit(self)
        # position_layout.addWidget(self.num_samples)

        central_widget = QWidget()
        central_widget.setLayout(layout)
        self.setCentralWidget(central_widget)

        self.load_button = QPushButton("Load Image", self)
        self.load_button.clicked.connect(self.load_image)
        layout.addWidget(self.load_button)

        self.calculate_buton = QPushButton("Calculate", self)
        self.calculate_buton.clicked.connect(self.calculate)
        layout.addWidget(self.calculate_buton)

        self.reset_button = QPushButton("Reset", self)
        self.reset_button.clicked.connect(self.reset)
        layout.addWidget(self.reset_button)

    def reset(self):
        self.cutoff_frequency.clear()
        self.frequency_shift.clear()
        self.image_label.selected_positions = []
        self.image_label.selected_box = QRect()

    def load_image(self):
        file_path, _ = QFileDialog.getOpenFileName(self, "Open Image File", "", "Image Files (*.png *.jpg *.jpeg *.bmp)")
        if file_path:
            pixmap = QPixmap(file_path)
            self.image_label.setPixmap(pixmap)

    def calculate(self):
        
        waterfall_size = self.image_label.geometry()
        waterfall_width = waterfall_size.width()
        waterfall_length = waterfall_size.height()

        waterfall_center = waterfall_width/2

        waterfall_sampling_rate = int(self.sampling_rate.text() or 0)
        #waterfall_samples = int(self.num_samples.text() or 0)
        
        waterfall_bandwidth=waterfall_sampling_rate/2

        if len(self.image_label.selected_positions) == 2:
            signal_frequency_span, signal_time_span = list(zip(*self.image_label.selected_positions))

            signal_frequency_span = sorted(signal_frequency_span)
            signal_time_span = sorted(signal_time_span)

            signal_center = (signal_frequency_span[0] + signal_frequency_span[1]) / 2
            signal_bandwidth = signal_frequency_span[1] - signal_frequency_span[0]

            signal_offset_normalized = (signal_center - waterfall_center) / (waterfall_width / 2)
            signal_offset_frequency = signal_offset_normalized * waterfall_bandwidth

            signal_cutoff_frequency_normalized = signal_bandwidth / waterfall_width
            signal_cutoff_frequency = signal_cutoff_frequency_normalized * waterfall_bandwidth


            signal_decimation_rate = int(waterfall_sampling_rate / signal_cutoff_frequency)
            
            output_sample_rate = waterfall_sampling_rate / signal_decimation_rate
            output_size_mb = output_sample_rate * 4 / (1024*1024) 

            self.frequency_shift.setText(str(round(signal_offset_frequency)))
            self.cutoff_frequency.setText(str(round(signal_cutoff_frequency)))
            self.output_sampling.setText(str(round(output_sample_rate)))
            self.output_size.setText(str(output_size_mb * 20))

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = ImageSelector()
    window.show()
    sys.exit(app.exec_())
