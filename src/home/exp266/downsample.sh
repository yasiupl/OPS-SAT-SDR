#!/usr/bin/env sh

# Author: Marcin Jasiukowicz <hi@y4s.io>
# Based on: https://github.com/emvivre/iq_toolbox/

echo "#### Downsampling started"

EXP_PATH=$(dirname $0)
BINARY_PATH=$EXP_PATH/bin
CONFIG_FILE=$EXP_PATH/config.ini
DATE=$(date +"%Y%m%d_%H%M%S")
OUT_FOLDER=${1:-"toGround/downsample_${DATE}"}
mkdir -p $OUT_FOLDER

downsample_shift=$(awk -F "=" '/downsample_shift/ {printf "%s",$2}' $CONFIG_FILE)
downsample_cutoff_frequency=$(awk -F "=" '/downsample_cutoff_frequency/ {printf "%s",$2}' $CONFIG_FILE)

## Static config
samp_freq_index_lookup="1.5 1.75 3.5 3 3.84 5 5.5 6 7 8.75 10 12 14 20 24 28 32 36 40 60 76.8 80" # MHz
lpf_bw_cfg_lookup="14 10 7 6 5 4.375 3.5 3 2.75 2.5 1.92 1.5 1.375 1.25 0.875 0.75" # MHz

## Decode metadata from filename:
echo "### Reading stored archive..."

stored_filename=$($EXP_PATH/helper/peek_emmc.sh | awk '{ printf "%s",$6 }')

echo "## Found recording: $stored_filename"

echo "### Reading metadata from filename..."

# Grep all parameter=value pairs
pairs=$(echo "$stored_filename" | grep -o "\(.*=[0-9.]*\)")

f_sampling_index=$(echo "$pairs" | grep -o "f_sampling_index=[0-9.]*" | cut -d'=' -f2)
lpf_index=$(echo "$pairs" | grep -o "lpf_index=[0-9.]*" | cut -d'=' -f2)
f_center=$(echo "$pairs" | grep -o "f_center=[0-9.]*" | cut -d'=' -f2)
gain=$(echo "$pairs" | grep -o "gain=[0-9.]*" | cut -d'=' -f2)
timestamp=$(echo "$pairs" | grep -o "timestamp=[0-9.]*" | cut -d'=' -f2)

# Decode indexes into real values

sampling_realvalue=$(echo $samp_freq_index_lookup | cut -d " " -f $(($f_sampling_index+1)))
lpf_realvalue=$(echo $lpf_bw_cfg_lookup | cut -d " " -f $(($lpf_index+1)))

# Print metadata

sampling_Hz=$(python3 -c "print(round($sampling_realvalue*1000000))")
decimation_rate=$(python3 -c "print(int($sampling_Hz/$downsample_cutoff_frequency))")
output_sample_rate=$(python3 -c "print(int($sampling_Hz/$decimation_rate))");
new_center=$(python3 -c "print($f_center*1000+$downsample_shift/1000000)")

MOTD="

  ## Stored recording parameters:

  Stored filename:  $stored_filename;
  Center frequency: $f_center GHz;
  Sampling Rate:    $sampling_realvalue MHz (id: $f_sampling_index);
  Low Pass filter:  $lpf_realvalue MHz (id: $lpf_index);
  Gain:             $gain dB;

  ## Downsampling parameters:

  Cutoff:           $downsample_cutoff_frequency Hz;
  Shift:            $downsample_shift Hz;
  New frequency:    $new_center MHz;
  New sampling rate:$output_sample_rate Hz;
  
"
echo "$MOTD"

filename=sdr_exp266_downsampled-f_center=${f_center}-f_shift=${downsample_shift}-f_sampling=${output_sample_rate}-timestamp=$DATE.cs16

echo "### Starting resampling to file: $filename"

## Works on EM:
$EXP_PATH/helper/stream_emmc.sh | tar -xvO | $BINARY_PATH/iq_toolbox/iq_mix -s $sampling_Hz -m $downsample_shift | $BINARY_PATH/iq_toolbox/iq_decimate -s $sampling_Hz -f $downsample_cutoff_frequency -o $OUT_FOLDER/$filename

downsample_waterfall=$(awk -F "=" '/downsample_waterfall/ {printf "%s",$2}' $CONFIG_FILE)
downsample_fft_size=$(awk -F "=" '/downsample_fft_size/ {printf "%s",$2}' $CONFIG_FILE)
#downsample_fft_size=$(python3 -c "print(int((2 ** (int($output_sample_rate) - 1).bit_length())/16))")
if [[ $downsample_waterfall == true ]]; then
  echo "### Generating waterfall..."
  $EXP_PATH/waterfall.sh $OUT_FOLDER/$filename $OUT_FOLDER $downsample_fft_size
fi

echo "#### Downsampling done, byebye!"