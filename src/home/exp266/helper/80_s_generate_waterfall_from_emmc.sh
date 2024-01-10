#!/bin/sh

downlink_path=/esoc-apps-flash/fms/filestore/toGround/

set -x

opkg install ~/exp266_*.ipk
cd /home/exp266
./helper/restore_from_emmc_tar.sh
./renderfall.sh toGround/restored_*/toGround/binary_* $downlink_path

mv toGround/restored_*/toGround/* $downlink_path

set +x