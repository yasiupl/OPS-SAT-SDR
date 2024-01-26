#!/usr/bin/env sh

$(dirname $0)/create_emmc_partition.sh

gnu_tar.tar tvf /dev/mmcblk0p180