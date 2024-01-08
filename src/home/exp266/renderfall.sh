#/bin/env sh

BINARY_PATH=./bin

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
WINDOW=$(awk -F "=" '/waterfall_window/ {print $2}' config.ini)

# SIZE=1024
# SIZE=2048
# SIZE=4096
# SIZE=8192
# SIZE=16384
SIZE=$(awk -F "=" '/waterfall_fft_size/ {print $2}' config.ini)

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

FILENAME=renderfall_${WINDOW}_${SIZE}
echo $FILENAME
ARGUMENTS="$IN_FILE --format int16 --fftsize $SIZE --window $WINDOW --outfile $OUT_FOLDER/$FILENAME.png"
echo $BINARY_PATH/$RENDERFALL_BINARY $ARGUMENTS
export LD_PRELOAD=lib/libfftw3.so.3
$BINARY_PATH/$RENDERFALL_BINARY $ARGUMENTS --verbose && $BINARY_PATH/pngtopnm -quiet $OUT_FOLDER/$FILENAME.png | $BINARY_PATH/ppmtojpeg -quiet > $OUT_FOLDER/$FILENAME.jpg
export LD_PRELOAD=""
