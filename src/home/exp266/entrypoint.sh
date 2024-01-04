#!/usr/bin/env sh

## TODO
# add script to tar to partition.
# add script to un-tar results from partition.
# modify the process_samples binary to store the data in partition directly OR print to STDOUT + divert to partition.

samp_freq_index_lookup="0.750 0.875 1.25 1.5 1.92 2.5 2.75 3 3.5 4.375 5 6 7 10 12 14 16 18 20 30 38.4 40" # MHz
lpf_bw_cfg_lookup="14 10 7 6 5 4.375 3.5 3 2.75 2.5 1.92 1.5 1.375 1.25 0.875 0.75" # MHz
HOME_DIR=$PWD
#id=$RANDOM
id=exp266
binary=process_samples
output_path=$PWD/toGround/$(ls -rt toGround/ | tail -n1)

# Read the config
config="config.ini"
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

    Authors: OPS-SAT MCT, Libre Space Foundation

  Center frequency: $carrier_frequency_GHz GHz;
  Sampling Rate:    $(echo $samp_freq_index_lookup | cut -d " " -f $(($samp_freq_index+1))) MHz (id: $samp_freq_index);
  Low Pass filter:  $(echo $lpf_bw_cfg_lookup | cut -d " " -f $(($lpf_bw_cfg))) MHz (id: $lpf_bw_cfg);
  Gain:             $gain_db dB;
  Output path:      $output_path
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
binary_path = $HOME_DIR/bin/$binary
output_path = $output_path/${id}_${binary}_"

echo "$CONFIG" > running_config.ini
#set -ex
## Setup FPGA firmware - devicetree
echo "Setup FPGA firmware - devicetree"
./helper/firmware_setup.sh

## Start recording
echo "Start Recording"
#export LD_PRELOAD="$HOME_DIR/lib/libfftw3.so.3;$HOME_DIR/lib/libsdr_api.so;$HOME_DIR/lib/libsepp_api_core.so;$HOME_DIR/lib/libsepp_ic.so"
./bin/sdr_sidlock_launcher running_config.ini
mv running_config.ini $output_path/
#export LD_PRELOAD=""

# WORKAROUND Rename the output to .cs16 format
sdr_recording_name=$(ls -rt $output_path | grep ${id}_${binary}_)
mv $output_path/$sdr_recording_name $output_path/${sdr_recording_name}.cs16
sdr_recording_name=${sdr_recording_name}.cs16
echo "Recording finished! File: $output_path/$sdr_recording_name"

## Store the recording to eMMC
store_recording_to_emmc=$(awk -F "=" '/store_recording_to_emmc/ {print $2}' $config)
if [[ $store_recording_to_emmc == true ]]; then
  echo "Store the recording to eMMC"
  ./helper/store_to_emmc_tar.sh $output_path
fi

## Process the recording
waterfall_render=$(awk -F "=" '/waterfall_render/ {print $2}' $config)
if [[ $waterfall_render == true ]]; then
  echo "Generate the waterfall"
  ./renderfall.sh $output_path/$sdr_recording_name $output_path
  renderfall_name=$(ls -rt $output_path/ | tail -n1)
  echo "Waterfall is not stored in eMMC. Generate it again after restoring using $PWD/renderfall.sh INPUT_FILE OUTPUT_PATH"
fi

## Zip for downlink
zip_and_downlink=$(awk -F "=" '/zip_and_downlink/ {print $2}' $config)
if [[ $zip_and_downlink == true ]]; then
  tar czvf /esoc-apps-flash/fms/filestore/toGround/exp266_sdr_results_$(date +"%Y%m%d_%H%M%S").tar.gz $output_path
fi

## Cleanup
echo "Cleaning up"
export LD_PRELOAD=""
keep_recording_in_filesystem=$(awk -F "=" '/keep_recording_in_filesystem/ {print $2}' $config)
if [[ $keep_recording_in_filesystem == false ]]; then
  rm $output_path/$sdr_recording_name
fi

#set +ex