#!/usr/bin/env sh

## Static config
partition=/dev/mmcblk0p180
downsample_config=config.ini
samp_freq_index_lookup="1.5 1.75 3.5 3 3.84 5 5.5 6 7 8.75 10 12 14 20 24 28 32 36 40 60 76.8 80" # MHz
#samp_freq_index_lookup="0.750 0.875 1.25 1.5 1.92 2.5 2.75 3 3.5 4.375 5 6 7 10 12 14 16 18 20 30 38.4 40" # MHz (half)
lpf_bw_cfg_lookup="14 10 7 6 5 4.375 3.5 3 2.75 2.5 1.92 1.5 1.375 1.25 0.875 0.75" # MHz

DATE=$(date +"%Y-%m-%dT%H-%M-%S")
OUTPUT=toGround/$DATE

mkdir -p $OUTPUT

## Dynamic config
downsample_shift=$(awk -F "=" '/downsample_shift/ {printf "%s",$2}' $downsample_config)
downsample_rate=$(awk -F "=" '/downsample_rate/ {printf "%s",$2}' $downsample_config)
downsample_waterfall=$(awk -F "=" '/downsample_waterfall/ {printf "%s",$2}' $downsample_config)


## Decode metadata from filename:
stored_filename=$(gnu_tar.tar tvf $partition | awk '{ print $6 }')

string=$stored_filename
regex="\(.*=[0-9.]*\)"

# Using grep to extract all pairs
pairs=$(echo "$string" | grep -o "$regex")

# Storing in variables (you can customize this part)
f_sampling_index=$(echo "$pairs" | grep -o "f_sampling_index=[0-9.]*" | cut -d'=' -f2)
lpf_index=$(echo "$pairs" | grep -o "lpf_index=[0-9.]*" | cut -d'=' -f2)
f_center=$(echo "$pairs" | grep -o "f_center=[0-9.]*" | cut -d'=' -f2)
gain=$(echo "$pairs" | grep -o "gain=[0-9.]*" | cut -d'=' -f2)
timestamp=$(echo "$pairs" | grep -o "timestamp=[0-9.]*" | cut -d'=' -f2)


sampling_realvalue=$(echo $samp_freq_index_lookup | cut -d " " -f $(($f_sampling_index+1)))
lpf_realvalue=$(echo $lpf_bw_cfg_lookup | cut -d " " -f $(($lpf_index+1)))


MOTD="

  ## Stored recording parameters:

  Stored filename:  $stored_filename;
  Center frequency: $f_center GHz  ($(python3 -c "print($f_center*1000)"));
  Sampling Rate:    $sampling_realvalue MHz (id: $f_sampling_index);
  Low Pass filter:  $lpf_realvalue MHz (id: $lpf_index);
  Gain:             $gain dB;

  ## Downsampling parameters:

  New frequency:    $(python3 -c "print((($f_center)+downsample_shift/1000000000))") GHz;
  Shift:            $(python3 -c "print($downsample_shift/1000)") kHz;
  Bandwidth         $(python3 -c "print($downsample_rate/1000)") kHz;
  
"
echo "$MOTD"

sampling_Hz=$(python3 -c "print($sampling_realvalue*1000000)")

filename=sdr_exp266_downsampled-f_c=${fc}-shift=${downsample_shift}-fs=${downsample_samplerate}_$DATE.cs16

## Works on EM:
dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832 | gnu_tar.tar -xvO | bin/iq_toolbox/iq_mix -s $sampling_Hz -m $downsample_shift | bin/iq_toolbox/iq_decimate -s $sampling_Hz -f $downsample_rate -o $OUTPUT/$filename

if [[ $downsample_waterfall == true ]]; then
  ./waterfall.sh $OUTPUT/$filename $OUTPUT
fi