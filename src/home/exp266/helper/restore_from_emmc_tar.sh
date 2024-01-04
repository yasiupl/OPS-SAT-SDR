#!/usr/bin/env sh
set +x

restored_date=$(date +"%Y%m%d_%H%M%S")
output_path=/home/exp266/toGround/restored_${restored_date}

mkdir -p $output_path

dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832 of=$output_path/sdr_recording_restored_${restored_date}.tar.gz
#dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832 | gnu_tar.tar xv -C $output_path
ls -lah /tmp

set -x