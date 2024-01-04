#!/bin/sh
exp_id="exp266"
NUM_PROCESSES=$(ps aux | tr -s " " | cut -d' ' -f5 | grep -i "$exp_id" | grep -vE 'grep|start|su' | wc -l)
timestamp_trigger=$(date +"%Y%m%d_%H%M%S")
logfile=$exp_id_$timestamp_trigger.log
HOME_DIR=$PWD

mkdir -p $HOME_DIR/toGround/$timestamp_trigger/

if [ $NUM_PROCESSES -ge 1 ]
then
    # Exit app, because it is already running
    echo "$exp_id is already running..."
else
    # Run app
    echo "$$" > $HOME_DIR/exp_pid
    echo "PID $(cat $HOME_DIR/exp_pid)"
    echo "Non-NMF experiment"
    echo "Starting $exp_id"
    $HOME_DIR/entrypoint.sh 2>&1 | awk '{print strftime("[%d-%m-%Y %H:%M:%S.%f]"), $0}' | tee -a $HOME_DIR/toGround/$timestamp_trigger/$logfile
    echo "$exp_id ended - exiting now"
fi

exit 0
