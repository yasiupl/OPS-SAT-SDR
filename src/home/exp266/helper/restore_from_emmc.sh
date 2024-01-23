#!/usr/bin/env sh

## Restore the recording from eMMC and put it into provided folder. If no path is provided, it will save in exp266 "toGround" folder.

set -x

restored_date=$(date +"%Y%m%d_%H%M%S")
output_path=${1:-"/home/exp266/toGround/restored_${restored_date}"}

mkdir -p $output_path
echo "### Files stored in partition:"
dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832 | gnu_tar.tar tv

echo "### Restoring from eMMC and unpacking to SEPP Filesystem here: $output_path"
#dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832 of=$output_path/sdr_recording_restored_${restored_date}.tar.gz
dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832 | gnu_tar.tar xv -C $output_path

echo "### Restored files:"
ls -lahR $output_path

set +x