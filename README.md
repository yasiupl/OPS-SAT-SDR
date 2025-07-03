![Authors' first picture from OPS-SAT camera subsystem](docs/img_msec_1693941828514_2_cs.png)

# OPS-SAT SDR: On-board Radio Signal Processing

This repository contains the source code and associated documentation for the Master’s Thesis: "Software Defined Radio Data Processing on Board OPS-SAT Space Laboratory". The project demonstrates a comprehensive software experiment that utilizes the OPS-SAT satellite's Software Defined Radio (SDR) subsystem for in-orbit data acquisition, storage, and processing. It provides the implementations needed for the complete operational cycle, from data capture to efficient downlink, using the Satellite Experimental Processing Platform (SEPP).

[Read the Master Thesis](docs/Software%20Defined%20Radio%20data%20processing%20on%20board%20OPS-SAT%20Space%20Laboratory%20-%20Master%20Thesis%2C%20Marcin%20Jasiukowicz%2C%20s176214.pdf)

[Read short promotional folder of the thesis (Polish)](docs/Jasiukowicz_master-thesis_promo-folder.pdf)

## Features

This project provides a streamlined and efficient workflow for handling SDR data on the OPS-SAT platform, addressing the challenges of limited resources and data downlink bottlenecks.

  * **In-Orbit Data Acquisition:** The `record` action allows for capturing raw IQ samples from the SDR.
  * **On-board Data Storage:** To overcome the limitations of the SEPP's RAM, the experiment writes the captured data directly to the eMMC persistent storage. This enables longer recording times. The data is stored in the TAR format, which includes metadata about the recording parameters.
  * **Efficient Data Downlink:** Instead of downlinking the entire raw data, which can be very large, the project offers a two-step process to significantly reduce the amount of data sent to the ground station:
    1.  **Waterfall Generation:** The `waterfall` action generates a lightweight spectrogram (waterfall plot) of the entire captured signal. This image can be quickly downlinked to provide a preview of the recorded spectrum.
    2.  **On-board Downsampling:** After analyzing the waterfall plot, the `downsample` action can be used to extract only the signals of interest from the full recording. This is achieved using a lightweight DSP toolbox to filter and downsample the data, resulting in a much smaller file that can be downlinked quickly.
  * **Signal Picker GUI Tool:** To simplify the process of selecting signals for downsampling, a Python-based GUI tool is provided in the `signal-picker` directory. This tool allows the user to load a waterfall image, select a signal of interest, and automatically calculate the required parameters for the `downsample` action.

## How it Works

The experiment is designed to be controlled via a simple configuration file (`config.ini`) and a main execution script (`entrypoint.sh`). The workflow is as follows:

1.  **Configuration:** The user configures the desired action (`record`, `waterfall`, or `downsample`) and its corresponding parameters in the `config.ini` file.
2.  **Execution:** The `entrypoint.sh` script is executed on the SEPP. It reads the configuration and triggers the appropriate action script (`record.sh`, `waterfall.sh`, or `downsample.sh`).
3.  **Recording:** If the `record` action is selected, the experiment captures the SDR data and stores it in the eMMC.
4.  **Preview:** The `waterfall` action can then be used to generate a waterfall plot from the stored data, which is then downlinked.
5.  **Downsampling:** The user can analyze the waterfall plot using the Signal Picker tool to determine the parameters for downsampling. These parameters are then used in the `config.ini` file to run the `downsample` action.
6.  **Downlink:** The final, much smaller, downsampled data is then downlinked.

## Repository Structure

```
├── automax/             # Automation scripts for the OPS-SAT Mission Control Team
├── dependencies/        # Dependencies for building and testing the project
├── delivery/            # Zipped experiment packages for deployment
├── docs/                # Project documentation, including the Master's Thesis
├── signal-picker/       # Python-based GUI tool for selecting signals from waterfall plots
├── src/                 # Source code for the experiment
│   └── home/
│       └── exp266/      # Main experiment directory
│           ├── bin/     # Compiled binaries
│           ├── helper/  # Helper scripts
│           ├── config.ini
│           ├── entrypoint.sh
│           ├── record.sh
│           ├── waterfall.sh
│           └── downsample.sh
├── from_osdrs.sh        # Script to list files from the OSDSR server
├── to_em.sh             # Script to copy files to the Engineering Model
├── to_osdrs.sh          # Script to upload the experiment package
└── zip_experiment.sh    # Script to package the experiment for deployment
```

## Core Dependencies
The exp266 software relies on these key components:

* [Sidloc SDR Driver](https://gitlab.com/librespacefoundation/ops-sat-sidloc/ops-sat-sidloc-sw): The core binary for recording, based on the work from the Libre Space Foundation's SIDLOC experiment, modified to support direct writing to the eMMC.

* [iq_toolbox](https://github.com/emvivre/iq_toolbox): A suite of lightweight, command-line signal processing tools used for filtering and downsampling the raw IQ data on board the satellite.

* [Renderfall](https://github.com/TomMladenov/renderfall): A tool used to generate waterfall diagrams (spectrograms) from the raw IQ samples to create previews of the recordings.

* [microtar](https://github.com/rxi/microtar): A lightweight C library used to create TAR archive headers programmatically, allowing metadata to be saved alongside the raw signal data on the eMMC partition.

## Real-World Application

The experiment was successfully tested and demonstrated in a real-world scenario in December 2023 during the 37th Chaos Communication Congress (37C3). A voice transmission from a handheld radio was successfully captured by OPS-SAT in orbit, and the recorded data was downlinked and processed using the tools from this repository. This test validated the entire workflow and showcased the project's capabilities in a live demonstration.

## Background

OPS-SAT is a 3-unit CubeSat launched by the European Space Agency (ESA) on December 18, 2019, into a 515 km polar, Sun-Synchronous Orbit (SSO). It serves as a flying laboratory for testing and demonstrating cutting-edge space technologies and operational concepts. The mission was conceived to address the "has never flown, will never fly" paradox in the space industry, which often hinders the adoption of innovative software due to the high risks and costs associated with space missions.

Key features of the OPS-SAT platform include:

  * **A Powerful Experimental Processor:** At its core is the Satellite Experimental Processing Platform (SEPP), which features a dual-core ARM Cortex-A9 CPU and a reconfigurable FPGA. This gives it approximately 10 times more computational power than previous ESA spacecraft.
  * **Advanced and Redundant Systems:** The satellite combines commercial-off-the-shelf (COTS) components with a redundant design to ensure mission safety and fault tolerance.
  * **Open Access for Experimenters:** It provides a safe and reconfigurable platform for external collaborators to run software-based experiments, allowing them to take control of the satellite to test new algorithms and operational techniques.

### Mission Conclusion
The OPS-SAT-1 mission concluded on May 22, 2024, when the satellite re-entered the Earth's atmosphere and burned up due to natural orbital decay. The satellite remained operational until its final moments, with its telemetry being tracked by the worldwide amateur radio community.

### Relevant Links

  * [ESA's Official OPS-SAT Page](https://opssat1.esoc.esa.int/)
  * [OPS-SAT Picture Gallery on Flickr](https://www.flickr.com/photos/esa_events/albums/72157716491073681/)
  * [Gunter's Space Page on OPS-SAT](https://space.skyrocket.de/doc_sdat/ops-sat.htm)
  * [OPS-SAT Reentry Dataset](https://opssat.esa.int/ops-sat-1/reentry_dataset/)
  * [gr-opssat: GNU Radio module for OPS-SAT](https://github.com/esa/gr-opssat)
