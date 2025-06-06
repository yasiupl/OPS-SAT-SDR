![Authors' first picture from OPS-SAT camera subsystem](docs/img_msec_1693941828514_2_cs.png)

# OPS-SAT SDR: On-board Radio Signal Processing

This repository contains the source code and associated documentation for the Master’s Thesis: "Software Defined Radio Data Processing on Board OPS-SAT Space Laboratory". The project demonstrates a comprehensive software experiment that utilizes the OPS-SAT satellite's Software Defined Radio (SDR) subsystem for in-orbit data acquisition, storage, and processing. It provides the implementations needed for the complete operational cycle, from data capture to efficient downlink, using the Satellite Experimental Processing Platform (SEPP).

[Read the Master Thesis](docs/Software%2520Defined%2520Radio%2520data%2520processing%2520on%2520board%2520OPS-SAT%2520Space%2520Laboratory%2520-%2520Master%2520Thesis,%2520Marcin%2520Jasiukowicz,%2520s176214.pdf)

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

## Real-World Application

The experiment was successfully tested and demonstrated in a real-world scenario in December 2023 during the 37th Chaos Communication Congress (37C3). A voice transmission from a handheld radio was successfully captured by OPS-SAT in orbit, and the recorded data was downlinked and processed using the tools from this repository. This test validated the entire workflow and showcased the project's capabilities in a live demonstration.