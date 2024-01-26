#!/bin/sh

exp_id="exp266"
DATE=$(date +"%Y%m%d_%H%M%S")
EXP_PATH=$(dirname $0)
NUM_PROCESSES=$(ps aux | tr -s " " | cut -d' ' -f5 | grep -i "$exp_id" | grep -vE 'grep|start|su' | wc -l)
EXECUTION_DIR=$EXP_PATH/toGround/$DATE
mkdir -p $EXECUTION_DIR
echo Created output directory: $EXECUTION_DIR

if [ $NUM_PROCESSES -ge 1 ]
then
    # Exit app, because it is already running
    echo "$exp_id is already running..."
else
    # Run app
    echo "$$" > $EXP_PATH/exp_pid
    echo "PID $(cat $EXP_PATH/exp_pid)"
    echo "Non-NMF experiment"
    echo "Starting $exp_id"
    logfile=${exp_id}_${DATE}.log
    $EXP_PATH/entrypoint.sh $EXECUTION_DIR 2>&1 | awk '{print strftime("[%d-%m-%Y %H:%M:%S.%f]"), $0}' | tee -a $EXECUTION_DIR/$logfile
    echo "$exp_id ended - exiting now"
fi

exit 0
