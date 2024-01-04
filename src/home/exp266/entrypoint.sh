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

echo "Set up firmware."
./helper/firmware_setup.sh

# Capture recording
echo "Start Recording"
#export LD_PRELOAD="$HOME_DIR/lib/libfftw3.so.3;$HOME_DIR/lib/libsdr_api.so;$HOME_DIR/lib/libsepp_api_core.so;$HOME_DIR/lib/libsepp_ic.so"
./bin/sdr_sidlock_start running_config.ini
#export LD_PRELOAD=""


sdr_recording_name=$(ls -rt $output_path | tail -n1)
echo "Recording finished! Filename: $sdr_recording_name"

#sleep 3

# Store the recording
echo "Storing the recording to eMMC"
./helper/store_to_emmc_tar.sh $sdr_recording_name

# Process the recording
echo "Generate the waterfall"
export LD_PRELOAD="$HOME_DIR/lib/libfftw3.so.3"
waterfall_fft_size=$(awk -F "=" '/waterfall_fft_size/ {print $2}' $config)
waterfall_window=$(awk -F "=" '/waterfall_window/ {print $2}' $config)
bin/renderfall $output_path/$sdr_recording_name --format int16 --fftsize $waterfall_fft_size --window $waterfall_window --outfile $output_path/sdr_waterfall_FFT-${waterfall_fft_size}_WINDOW-${waterfall_window}.png
renderfall_name=$(ls -rt $output_path/ | tail -n1)

# Cleanup
echo "Finished, cleaning up"
#export LD_PRELOAD=""
mv running_config.ini $output_path/

set +ex