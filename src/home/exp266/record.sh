#!/usr/bin/env sh

## TODO
# modify the process_samples binary to store the data in partition directly OR print to STDOUT + divert to partition.
# calculate the real size of the recording based on input number_of_samples
# check available space and notify when it is not enough to store the samples
# note the start and end date of the recording

EXP_PATH=$(dirname $0)
BINARY_PATH=$EXP_PATH/bin
LIB_PATH=$EXP_PATH/lib
DATE=$(date +"%Y%m%d_%H%M%S")
OUTPUT_PATH=${1:-"${EXP_PATH}/toGround/recording_$DATE"}
RECORDING_PATH=/dev/mmcblk0p180

config="config.ini"
id=exp266
binary=exp202-tar_write
sdr_sample_name_slug="${id}_${binary}_"

# Read the config
carrier_frequency_GHz=$(awk -F "=" '/carrier_frequency_GHz/ {printf "%s",$2}' $config)
samp_freq_index=$(awk -F "=" '/samp_freq_index/ {printf "%s",$2}' $config)
lpf_bw_cfg=$(awk -F "=" '/lpf_bw_cfg/ {printf "%s",$2}' $config)
gain_db=$(awk -F "=" '/gain_db/ {printf "%s",$2}' $config)
number_of_samples=$(awk -F "=" '/number_of_samples/ {printf "%s",$2}' $config)
calibrate_frontend=$(awk -F "=" '/calibrate_frontend/ {printf "%s",$2}' $config)

## MOTD

samp_freq_index_lookup="1.5 1.75 3.5 3 3.84 5 5.5 6 7 8.75 10 12 14 20 24 28 32 36 40 60 76.8 80" # MHz
lpf_bw_cfg_lookup="14 10 7 6 5 4.375 3.5 3 2.75 2.5 1.92 1.5 1.375 1.25 0.875 0.75" # MHz

sampling_realvalue=$(echo $samp_freq_index_lookup | cut -d " " -f $(($samp_freq_index+1)))
lpf_realvalue=$(echo $lpf_bw_cfg_lookup | cut -d " " -f $(($lpf_bw_cfg+1)))


MOTD="
   ___  ___  ___     ___   _ _____   ___ ___  ___   
  / _ \| _ \/ __|___/ __| /_\_   _| / __|   \| _ \  
 | (_) |  _/\__ \___\__ \/ _ \| |   \__ \ |) |   /  
  \___/|_|  |___/   |___/_/ \_\_|   |___/___/|_|_\  

                      Authors: 
    TU-Graz, OPS-SAT MCT, Libre Space Foundation
                       et al. 

  Center frequency: $carrier_frequency_GHz GHz $(python3 -c "print($carrier_frequency_GHz*1000)")MHz;
  Sampling Rate:    $sampling_realvalue MHz (id: $samp_freq_index);
  Low Pass filter:  $lpf_realvalue MHz (id: $lpf_bw_cfg);
  Gain:             $gain_db dB;
  Calibrate:        $calibrate_frontend

  Recording path:   $RECORDING_PATH
  Output path:      $OUTPUT_PATH
  Expected size:    $(python3 -c "print(($number_of_samples*4)/(1024*1024))")MB
  Samples:          $number_of_samples
  
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

set -e

## Setup FPGA firmware - devicetree
echo "#### Setup FPGA firmware - devicetree."
$EXP_PATH/helper/firmware_setup.sh

## Setup eMMC partition
echo "#### Setup eMMC partition."
wipe_partition=$(awk -F "=" '/wipe_partition/ {printf "%s",$2}' $config)
if [[ $wipe_partition == true ]]; then
    echo "#### Wiping partition clean. It make take a while, account for this in planning!"
    $EXP_PATH/helper/create_emmc_partition.sh wipe_partition
    else
    $EXP_PATH/helper/create_emmc_partition.sh
fi

## Start recording
echo "#### Start Recording."
#export LD_PRELOAD="$LIB_PATH/libfftw3.so.3;$LIB_PATH/libsdr_api.so;$LIB_PATH/libsepp_api_core.so;$LIB_PATH/libsepp_ic.so"
$BINARY_PATH/$binary running_config.ini
mv running_config.ini $OUTPUT_PATH/
#export LD_PRELOAD=""

echo "#### Recording finished! File: $RECORDING_PATH"

set +e

## Process the recording
waterfall_render=$(awk -F "=" '/waterfall_render/ {printf "%s",$2}' $config)
if [[ $waterfall_render == true ]]; then
  echo "#### Generate the waterfall."
  $EXP_PATH/waterfall.sh $RECORDING_PATH $OUTPUT_PATH
  waterfall_name=$(ls -rt $OUTPUT_PATH/ | tail -n1)
  echo "#### Waterfall generated: $waterfall_name"
fi

## Cleanup
echo "#### Cleaning up"
export LD_PRELOAD=""