#!/bin/sh

#$(dirname $0)/create_emmc_partition.sh

dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832