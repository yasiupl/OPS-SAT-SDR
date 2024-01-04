#/bin/env sh

IN_FILE=$1
OUT_FOLDER=$2

## renderfall command
BINARY=bin/renderfall
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
#SIZE=8192
# SIZE=16384
SIZE=$(awk -F "=" '/waterfall_fft_size/ {print $2}' config.ini)

DATE=$(date +"%Y-%m-%dT%H-%M-%S")

if [ -z $IN_FILE ]; then
    echo "No input file given"
    exit 2
fi

if [ -z $OUT_FILE ]; then
    OUT_FOLDER=toGround/${OUT_FILE}_${DATE}
    mkdir -p $OUT_FOLDER
    echo "No output file name given. Defaulting to \"$OUT_FILE\"."
fi

FILENAME=${OUT_FILE}_${WINDOW}_${SIZE}
echo $FILENAME
ARGUMENTS="$IN_FILE --format int16 --fftsize $SIZE --window $WINDOW --outfile $OUT_FOLDER/$FILENAME.png"
echo $BINARY $ARGUMENTS
export LD_PRELOAD=lib/libfftw3.so.3
$BINARY $ARGUMENTS --verbose
export LD_PRELOAD=""
