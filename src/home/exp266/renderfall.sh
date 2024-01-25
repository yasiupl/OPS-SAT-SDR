#/bin/env sh

EXP_PATH=/home/exp266
BINARY_PATH=$EXP_PATH/bin
LIB_PATH=$EXP_PATH/lib
CONFIG_FILE=$EXP_PATH/config.ini

IN_FILE=$1
OUT_FOLDER=$2

## renderfall command
RENDERFALL_BINARY=renderfall
# BINARY=./renderfall_rgb
# BINARY=./renderfall_bw
# BINARY=renderfall

## Available FFT Windows
# WINDOWS=(hann gaussian square blackmanharris hamming kaiser parzen)
# WINDOWS=(blackmanharris hamming)
#WINDOW=blackmanharris
WINDOW=$(awk -F "=" '/waterfall_window/ {print $2}' $CONFIG_FILE)

# SIZE=1024
# SIZE=2048
# SIZE=4096
# SIZE=8192
# SIZE=16384
SIZE=$(awk -F "=" '/waterfall_fft_size/ {print $2}' $CONFIG_FILE)

DATE=$(date +"%Y-%m-%dT%H-%M-%S")

if [ -z $IN_FILE ]; then
    echo "No input file given"
    exit 2
fi

if [ -z $OUT_FOLDER ]; then
    OUT_FOLDER=toGround/renderfall_${DATE}
    mkdir -p $OUT_FOLDER
    echo "No output folder given. Defaulting to \"$OUT_FOLDER\"."
fi

FILENAME=renderfall_${WINDOW}_${SIZE}_${DATE}
echo Filename: $FILENAME
ARGUMENTS="$IN_FILE --format int16 --fftsize $SIZE --window $WINDOW --outfile $OUT_FOLDER/$FILENAME.png"
echo $BINARY_PATH/$RENDERFALL_BINARY $ARGUMENTS
export LD_PRELOAD=$LIB_PATH/libfftw3.so.3
echo "### Generating waterfall"
$BINARY_PATH/$RENDERFALL_BINARY $ARGUMENTS --verbose

waterfall_convert_to_jpg=$(awk -F "=" '/waterfall_convert_to_jpg/ {print $2}' config.ini)
if [[ $waterfall_convert_to_jpg == true ]]; then
    echo "### Converting to JPG"
    export LD_PRELOAD=$LIB_PATH/libnetpbm.so.10
    $BINARY_PATH/pngtopnm -quiet $OUT_FOLDER/$FILENAME.png | $BINARY_PATH/ppmtojpeg -quiet > $OUT_FOLDER/$FILENAME.jpg
    rm $OUT_FOLDER/$FILENAME.png
fi
export LD_PRELOAD=""
