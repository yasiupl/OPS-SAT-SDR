#!/usr/bin/env sh

EXP_PATH=$(dirname $0)
LIB_PATH=$EXP_PATH/lib
BINARY_PATH=$EXP_PATH/bin
CONFIG_FILE=$EXP_PATH/config.ini
DATE=$(date +"%Y%m%d_%H%M%S")
RECORDING_PATH=$(awk -F "=" '/recording_path/ {printf "%s",$2}' $CONFIG_FILE)
OUTPUT_PATH=${1:-"$EXP_PATH/toGround/$DATE"}
DOWNLINK_PATH=$(awk -F "=" '/downlink_path/ {printf "%s",$2}' $CONFIG_FILE)
mkdir -p $DOWNLINK_PATH

## Run one of the routines:

action=$(awk -F "=" '/action/ {printf "%s",$2}' $CONFIG_FILE)


MOTD="
   ___  ___  ___     ___   _ _____   ___ ___  ___   
  / _ \| _ \/ __|___/ __| /_\_   _| / __|   \| _ \  
 | (_) |  _/\__ \___\__ \/ _ \| |   \__ \ |) |   /  
  \___/|_|  |___/   |___/_/ \_\_|   |___/___/|_|_\  

                      Authors: 
    TU-Graz, OPS-SAT MCT, Libre Space Foundation
                       et al. 
"
echo "$MOTD"
echo "Executing: $action"

if [[ "$action" == "record" ]]; then
  $EXP_PATH/record.sh $OUTPUT_PATH
fi

if [[ "$action" == "waterfall" ]]; then
  $EXP_PATH/waterfall.sh $RECORDING_PATH $OUTPUT_PATH
fi

if [[ "$action" == "downsample" ]]; then
  $EXP_PATH/downsample.sh $OUTPUT_PATH
fi

## Downlink
downlink_samples=$(awk -F "=" '/downlink_samples/ {printf "%s",$2}' $CONFIG_FILE)

## Downlink from eMMC
if [[ $downlink_samples == true ]]; then
    echo "#### Restore samples from eMMC and put for downlink."
    $EXP_PATH/helper/downlink_from_emmc.sh $DOWNLINK_PATH
else
    echo "#### Samples stored in the eMMC. To downlink them later, run 'cd /home/exp266; ./helper/downlink_from_emmc.sh'"
fi

downlink_to_ground=$(awk -F "=" '/downlink_to_ground/ {printf "%s",$2}' $CONFIG_FILE)
if [[ $downlink_to_ground == true ]]; then
  filename="exp266_sdr_${action}_${DATE}.tar.gz"
  echo "#### Compress and move to downlink folder. Filename: $filename"
  tar cfzv $DOWNLINK_PATH/$filename $OUTPUT_PATH
  echo "#### Removing original folder."
  rm -r $OUTPUT_PATH
  echo "#### Downlink folder:"
  ls -lhR $DOWNLINK_PATH
fi
