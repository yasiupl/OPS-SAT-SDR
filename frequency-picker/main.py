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
        
        if len(self.selected_positions) > 2:
            self.selected_positions = []
            self.selected_box = QRect()

        self.main_window().calculate()


class ImageSelector(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Image Selector")
        self.setFixedSize(1800, 1600)

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

        self.frequency_shift_label = QLabel("Resampling shift [Hz]", self)
        position_layout.addWidget(self.frequency_shift_label)

        self.frequency_shift = QLineEdit(self)
        position_layout.addWidget(self.frequency_shift)
        self.frequency_shift.setReadOnly(True)


        self.cutoff_frequency_label = QLabel("Resampling cut-off frequency [Hz]", self)
        position_layout.addWidget(self.cutoff_frequency_label)

        self.cutoff_frequency = QLineEdit(self)
        position_layout.addWidget(self.cutoff_frequency)
        self.cutoff_frequency.setReadOnly(True)


        self.sampling_rate_label = QLabel("Sampling Rate", self)
        position_layout.addWidget(self.sampling_rate_label)

        self.sampling_rate = QLineEdit(self)
        position_layout.addWidget(self.sampling_rate)

        self.num_samples_label = QLabel("Number of Samples", self)
        position_layout.addWidget(self.num_samples_label)

        self.num_samples = QLineEdit(self)
        position_layout.addWidget(self.num_samples)

        central_widget = QWidget()
        central_widget.setLayout(layout)
        self.setCentralWidget(central_widget)

        self.load_button = QPushButton("Load Image", self)
        self.load_button.clicked.connect(self.load_image)
        layout.addWidget(self.load_button)

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
        waterfall_samples = int(self.num_samples.text() or 0)
        
        waterfall_bandwidth=waterfall_sampling_rate/2

        if len(self.image_label.selected_positions) == 2:
            signal_frequency_span, signal_time_span = list(zip(*self.image_label.selected_positions))

            signal_frequency_span = sorted(signal_frequency_span)
            signal_time_span = sorted(signal_time_span)

            signal_center = (signal_frequency_span[0] + signal_frequency_span[1]) / 2
            signal_bandwidth = signal_frequency_span[1] - signal_frequency_span[0]

            signal_offset_normalized = (signal_center - waterfall_center) / (waterfall_width / 2)
            signal_offset_frequency = signal_offset_normalized * (waterfall_bandwidth / 2)

            signal_cutoff_frequency_normalized = (signal_bandwidth/2) / (waterfall_width / 2)
            signal_cutoff_frequency = signal_cutoff_frequency_normalized * (waterfall_bandwidth / 2)

            self.frequency_shift.setText(str(signal_offset_frequency))

            self.cutoff_frequency.setText(str(signal_cutoff_frequency))

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = ImageSelector()
    window.show()
    sys.exit(app.exec_())
