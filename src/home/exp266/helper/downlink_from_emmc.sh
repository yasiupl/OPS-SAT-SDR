#!/usr/bin/env sh
set +x

downlink=/esoc-apps-flash/fms/filestore/toGround
D=$(date +"%Y%m%d_%H%M")
name="${1:-sdr_restored_recording_$D}"

#tar cf - A | gzip -9 > B.tar.gz
dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832 of=$downlink/$name
#dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832 | gzip -9 > $downlink/$name
ls -lah $downlink

set -x