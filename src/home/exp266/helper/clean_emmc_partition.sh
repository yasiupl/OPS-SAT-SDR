#!/bin/sh

blkpg-part add /dev/mmcblk0 180 7004487680 192937984
sleep 2
dd if=/dev/zero of=/dev/mmcblk0p180 bs=4096 count=50688