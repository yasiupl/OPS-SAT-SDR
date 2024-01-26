#!/usr/bin/env sh

## The script will restore the recording from eMMC and compress it for downlink, without creating intermediary files. It takes a lot of time to compress, so run it in background (start with & at the end).

$(dirname $0)/create_emmc_partition.sh

#set -x

DOWNLINK_PATH=${1:-"/esoc-apps-flash/fms/filestore/toGround"}
mkdir -p $DOWNLINK_PATH

echo "### Reading stored archive..."

stored_filename=$($(dirname $0)/peek_emmc.sh | awk '{ printf "%s",$6 }')

echo "## Found recording: $stored_filename"
echo "### Restoring to $DOWNLINK_PATH"

$(dirname $0)/stream_emmc.sh | tar -xvO | gzip -1 -v > $DOWNLINK_PATH/exp266_restored_${stored_filename}.gz

# echo "### Content of the restored file:"
# gnu_tar.tar tvf $DOWNLINK_PATH/$stored_filename.tar.gz

echo "### Content of the downlink folder:"
ls -lhR $DOWNLINK_PATH

#set +x