#/bin/env sh

# Author: Marcin Jasiukowicz <hi@y4s.io>
# Based on: https://github.com/TomMladenov/renderfall

EXP_PATH=$(dirname $0)
BINARY_PATH=$EXP_PATH/bin
LIB_PATH=$EXP_PATH/lib
CONFIG_FILE=$EXP_PATH/config.ini
RECORDING_PATH=/dev/mmcblk0p180
DATE=$(date +"%Y%m%d_%H%M%S")

IN_FILE=${1:-$RECORDING_PATH}
OUT_FOLDER=${2:-"toGround/renderfall_${DATE}"}


waterfall_window=$(awk -F "=" '/waterfall_window/ {printf "%s",$2}' $CONFIG_FILE)
waterfall_fft_size=$(awk -F "=" '/waterfall_fft_size/ {printf "%s",$2}' $CONFIG_FILE)
waterfall_convert_to_jpg=$(awk -F "=" '/waterfall_convert_to_jpg/ {printf "%s",$2}' $CONFIG_FILE)

FFT=${3:-"$waterfall_fft_size"}

$EXP_PATH/helper/create_emmc_partition.sh

echo "### Generating waterfall"
FILENAME=renderfall_${waterfall_window}_${FFT}_${DATE}
ARGUMENTS="$IN_FILE --format int16 --fftsize $FFT --window $waterfall_window --outfile $OUT_FOLDER/$FILENAME.png"
echo "$BINARY_PATH/renderfall $ARGUMENTS"

export LD_PRELOAD=$LIB_PATH/libfftw3.so.3
$BINARY_PATH/renderfall $ARGUMENTS --verbose

if [[ $waterfall_convert_to_jpg == true ]]; then
    echo "### Converting to JPG"
    export LD_PRELOAD=$LIB_PATH/libnetpbm.so.10
    $BINARY_PATH/pngtopnm -quiet $OUT_FOLDER/$FILENAME.png | $BINARY_PATH/ppmtojpeg -quiet > $OUT_FOLDER/$FILENAME.jpg
    rm $OUT_FOLDER/$FILENAME.png
fi
export LD_PRELOAD=""
