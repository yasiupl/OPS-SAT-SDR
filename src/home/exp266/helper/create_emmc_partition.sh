#!/bin/sh

set -ex

format=${1:-"false"}

#/usr/sbin/blkpg-part add /dev/mmcblk0 180 7004487680 192937984
partition=180
start_address=7004487680
length=192937984

#dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832 | gnu_tar.tar xv -C /tmp 
bs=512
start_add_512=13680640 
lenght_512=376832

check_partition() {
    local partition_number=$1
    if [ -e "/dev/mmcblk0p$partition_number" ]; then
        echo "Device /dev/mmcblk0p$partition_number exists."
        return 0  # Device exists
    else
        echo "Device /dev/mmcblk0p$partition_number does not exist."
        return 1  # Device does not exist
    fi
}

create_partition() {
    local partition_number=$1
    local start=$2
    local size=$3
    echo "Creating partition $partition_number"
    
    blkpg-part add /dev/mmcblk0 $partition_number $start $size
    sleep 2
}

format_partition() {
    # Align to 4096 blocks (actual filesystem blocks)
    echo "Formating partition"
    dd if=/dev/zero of=/dev/mmcblk0p$partition_number bs=4096 count=$(($lenght_512 / 8))
}

check_or_create_partition() {
        check_partition $partition
    if [ $? -eq 0 ]; then
        echo "Partition $partition exists!"
    else
        create_partition $partition $start_address $length
    fi

    if [ $format = "wipe_partition" ]; then
        format_partition
    fi
}

set +ex

check_or_create_partition