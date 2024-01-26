#!/usr/bin/env sh

## The script will restore the recording from eMMC and compress it for downlink, without creating intermediary files. It takes a lot of time to compress, so run it in background (start with & at the end).

$(dirname $0)/create_emmc_partition.sh

set -x

restored_date=$(date +"%Y%m%d_%H%M%S")
output_path=${1:-"/esoc-apps-flash/fms/filestore/toGround"}

mkdir -p $output_path

#tar cf - A | gzip -9 > B.tar.gz
#dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832 of=$output_path/$name.tar
stored_filename=$($(dirname $0)/peek_partition.sh | awk '{ print $6 }')
echo "Stored file: $stored_filename"
$(dirname $0)/stream_partition.sh | gzip -1 -v > $output_path/$stored_filename.tar.gz
gnu_tar.tar tvf $output_path/$name.tar.gz
ls -lah $output_path

set +x