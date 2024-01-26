#!/bin/sh

$(dirname $0)/create_emmc_partition.sh

dd if=/dev/mmcblk0p180