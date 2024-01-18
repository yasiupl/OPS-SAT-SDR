#!/usr/bin/env sh

## TODO
# modify the process_samples binary to store the data in partition directly OR print to STDOUT + divert to partition.
# calculate the real size of the recording based on input number_of_samples
# check available space and notify when it is not enough to store the samples
# note the start and end date of the recording

HOME_DIR=$PWD
OUTPUT_SLUG=$(ls -rt $HOME_DIR/toGround/ | tail -n1)
OUTPUT_PATH=${1:-"${HOME_DIR}/toGround/${OUTPUT_SLUG}"}
RECORDING_PATH=/dev/mmcblk0p180
DOWNLINK_PATH=/esoc-apps-flash/fms/filestore/toGround/

config="config.ini"
id=exp266
binary=exp202_single_exec_to_file
sdr_sample_name_slug="${id}_${binary}_"

samp_freq_index_lookup="1.5 1.75 3.5 3 3.84 5 5.5 6 7 8.75 10 12 14 20 24 28 32 36 40 60 76.8 80" # MHz
#samp_freq_index_lookup="0.750 0.875 1.25 1.5 1.92 2.5 2.75 3 3.5 4.375 5 6 7 10 12 14 16 18 20 30 38.4 40" # MHz (half)
lpf_bw_cfg_lookup="14 10 7 6 5 4.375 3.5 3 2.75 2.5 1.92 1.5 1.375 1.25 0.875 0.75" # MHz

# Read the config
carrier_frequency_GHz=$(awk -F "=" '/carrier_frequency_GHz/ {print $2}' $config)
samp_freq_index=$(awk -F "=" '/samp_freq_index/ {print $2}' $config)
lpf_bw_cfg=$(awk -F "=" '/lpf_bw_cfg/ {print $2}' $config)
gain_db=$(awk -F "=" '/gain_db/ {print $2}' $config)
number_of_samples=$(awk -F "=" '/number_of_samples/ {print $2}' $config)
calibrate_frontend=$(awk -F "=" '/calibrate_frontend/ {print $2}' $config)

MOTD="
   ___  ___  ___     ___   _ _____   ___ ___  ___   
  / _ \| _ \/ __|___/ __| /_\_   _| / __|   \| _ \  
 | (_) |  _/\__ \___\__ \/ _ \| |   \__ \ |) |   /  
  \___/|_|  |___/   |___/_/ \_\_|   |___/___/|_|_\  

                      Authors: 
    TU-Graz, OPS-SAT MCT, Libre Space Foundation
                       et al. 

  Center frequency: $carrier_frequency_GHz GHz;
  Sampling Rate:    $(echo $samp_freq_index_lookup | cut -d " " -f $(($samp_freq_index+1))) MHz (id: $samp_freq_index);
  Low Pass filter:  $(echo $lpf_bw_cfg_lookup | cut -d " " -f $(($lpf_bw_cfg))) MHz (id: $lpf_bw_cfg);
  Gain:             $gain_db dB;
  Calibrate:        $calibrate_frontend

  Recording path:   $RECORDING_PATH
  Output path:      $OUTPUT_PATH/$OUTPUT_SLUG
"
echo "$MOTD"

# Generate running config for exp202
CONFIG="[SEPP_SDR_RX]
carrier_frequency_GHz = $carrier_frequency_GHz
samp_freq_index = $samp_freq_index
lpf_bw_cfg = $lpf_bw_cfg
gain_db = $gain_db
number_of_samples = $number_of_samples
calibrate_frontend = $calibrate_frontend
output_path = $RECORDING_PATH"

echo "$CONFIG" > running_config.ini

#set -ex

## Setup FPGA firmware - devicetree
echo "#### Setup FPGA firmware - devicetree."
./helper/firmware_setup.sh

## Start recording
echo "#### Start Recording."
#export LD_PRELOAD="$HOME_DIR/lib/libfftw3.so.3;$HOME_DIR/lib/libsdr_api.so;$HOME_DIR/lib/libsepp_api_core.so;$HOME_DIR/lib/libsepp_ic.so"
#./bin/sdr_sidlock_launcher running_config.ini
./bin/${single_binary} running_config.ini
mv running_config.ini $OUTPUT_PATH/
#export LD_PRELOAD=""

# WORKAROUND Rename the output to .cs16 format
# sdr_recording_name=$(ls -rt $OUTPUT_PATH | grep $sdr_sample_name_slug)
# mv $OUTPUT_PATH/$sdr_recording_name $OUTPUT_PATH/${sdr_recording_name}.cs16
# sdr_recording_name=${sdr_recording_name}.cs16
# echo "#### Recording finished! File: $OUTPUT_PATH/$sdr_recording_name"

echo "#### Recording finished! File: $RECORDING_PATH"


## Process the recording
waterfall_render=$(awk -F "=" '/waterfall_render/ {print $2}' $config)
if [[ $waterfall_render == true ]]; then
  echo "#### Generate the waterfall."
  ./renderfall.sh $RECORDING_PATH $OUTPUT_PATH
  renderfall_name=$(ls -rt $OUTPUT_PATH/ | tail -n1)
fi

## Downlink
downlink_samples=$(awk -F "=" '/downlink_samples/ {print $2}' $config)

## Downlink from eMMC
if [[ $downlink_samples == true ]]; then
    echo "#### Restore samples from eMMC and put for downlink."
    dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832 of=$OUTPUT_PATH/sdr_carrier-${carrier_frequency_GHz}GHz_sampling-${samp_freq_index}_lpf-${lpf_bw_cfg}_$OUTPUT_SLUG.cs16
fi

tar_downlink=$(awk -F "=" '/tar_downlink/ {print $2}' $config)
if [[ $tar_downlink == true ]]; then
  echo "#### Compress and move to downlink folder. Filename: exp266_sdr_${OUTPUT_SLUG}.tar.gz"
  tar cfzv $DOWNLINK_PATH/exp266_sdr_${OUTPUT_SLUG}.tar.gz $OUTPUT_PATH
  echo "#### Downlink folder:"
  ls -lhR $DOWNLINK_PATH
fi

## Cleanup
echo "#### Cleaning up"
export LD_PRELOAD=""
if [[ $keep_recording_in_filesystem == false ]]; then
  echo "#### Delete recording from filesystem."
  rm $OUTPUT_PATH/$sdr_recording_name
fi  

#set +ex