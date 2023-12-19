#!/usr/bin/env sh

## TODO
# add script to tar to partition.
# add script to un-tar results from partition.
# modify the process_samples binary to store the data in partition directly OR print to STDOUT + divert to partition.

set -ex

HOME_DIR=$PWD
#id=$RANDOM
id=exp266

#./setup.sh

# Reconfigure the experiment
mkdir -p output

# CONFIG="[SEPP_SDR_RX]
# carrier_frequency_GHz = 0.4352
# samp_freq_index = 0
# lpf_bw_cfg = 15
# gain_db = 60
# number_of_samples = 25000000
# calibrate_frontend = 0
# binary_path = $HOME_DIR/bin/process_samples
# output_path = output/sidlock_$id_"
# echo "$CONFIG" > config.ini

# Capture recording
echo "Starting Recording"
#export LD_PRELOAD="$HOME_DIR/lib/libfftw3.so.3;$HOME_DIR/lib/libsdr_api.so;$HOME_DIR/lib/libsepp_api_core.so;$HOME_DIR/lib/libsepp_ic.so"
./bin/sdr_sidlock_start config.ini
export LD_PRELOAD=""


sdr_recording_name=$(ls -rt output/ | tail -n1)
echo "Recording finished! result: $sdr_recording_name"

sleep 3

# Store the recording
echo "Storing the recording to eMMC"
./helper/store_to_emmc.sh $sdr_recording_name

# Process the recording
echo "Generate the waterfall"
#export LD_PRELOAD="$HOME_DIR/lib/libfftw3.so.3"
./renderfall.sh output/sidlock_* sdr_$id
renderfall_name=$(ls -rt toGround/ | tail -n1)

# Cleanup
echo "Finished, cleaning up"
#export LD_PRELOAD=""
rm -r output/*
mv config.ini toGround/$renderfall_name

set +ex