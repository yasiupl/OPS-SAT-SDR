#!/bin/sh
exp_id="exp266"
NUM_PROCESSES=$(ps aux | tr -s " " | cut -d' ' -f5 | grep -i "$exp_id" | grep -vE 'grep|start|su' | wc -l)
timestamp_trigger=$(date +"%Y%m%d_%H%M")
logfile=$exp_id_$timestamp_trigger.log
HOME_DIR=$PWD

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
    $HOME_DIR/start.sh | awk '{print strftime("[%d-%m-%Y %H:%M:%S.%f]"), $0}' >> $HOME_DIR/toGround/$logfile
    echo "$exp_id ended - exiting now"
fi

exit 0
