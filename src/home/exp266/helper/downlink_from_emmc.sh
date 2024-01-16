#!/usr/bin/env sh

## The script will restore the recording from eMMC and compress it for downlink, without creating intermediary files. It takes a lot of time to compress, so run it in background (start with & at the end).

set -x

restored_date=$(date +"%Y%m%d_%H%M%S")
output_path=/esoc-apps-flash/fms/filestore/toGround
name=${1:-sdr_recording_restored_${restored_date}}

mkdir -p $output_path

#tar cf - A | gzip -9 > B.tar.gz
#dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832 of=$output_path/$name.tar
dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832 | gzip -1 -v > $output_path/$name.tar.gz
gnu_tar.tar tvf $output_path/$name.tar.gz
ls -lah $output_path

set +x