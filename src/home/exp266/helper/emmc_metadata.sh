#!/usr/bin/env sh


## Static config
samp_freq_index_lookup="1.5 1.75 3.5 3 3.84 5 5.5 6 7 8.75 10 12 14 20 24 28 32 36 40 60 76.8 80" # MHz
lpf_bw_cfg_lookup="14 10 7 6 5 4.375 3.5 3 2.75 2.5 1.92 1.5 1.375 1.25 0.875 0.75" # MHz

## Decode metadata from filename:
echo "### Reading stored archive..."

stored_filename=$($(dirname $0)/peek_emmc.sh | awk '{ printf "%s",$6 }')

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
MOTD="

  ## Stored recording parameters:

  Stored filename:  $stored_filename;
  Center frequency: $f_center GHz;
  Sampling Rate:    $sampling_realvalue MHz (id: $f_sampling_index);
  Low Pass filter:  $lpf_realvalue MHz (id: $lpf_index);
  Gain:             $gain dB;
  
"
echo "$MOTD"