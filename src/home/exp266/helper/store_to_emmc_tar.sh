#!/bin/sh

set -ex

#predef_md5sum="0409f31cbfa6748c2a79ddeb769247d7"
file_path=$1

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

copy_and_verify() {
    local source_file=$1
    local destination_partition=$2
    # if [ "$predef_md5sum" == "$(md5sum $source_file | cut -d ' ' -f 1)" ]; then
        su - -c "cd $(dirname $source_file); /usr/sbin/gnu_tar.tar cvf /dev/mmcblk0p$destination_partition $(basename $source_file)"
        echo 3 > /proc/sys/vm/drop_caches
        # list files in stored tar
        echo "Stored to partition:"
        dd if=/dev/mmcblk0 bs=$bs skip=$start_add_512 count=$lenght_512 | gnu_tar.tar tv
    # else
    #     echo "MD5 checksum does not match. Skipping the storing and verification."
    # fi
}

main() {
    check_partition $partition
    if [ $? -eq 0 ]; then
        echo "Partition $partition exists!"
    else
        create_partition $partition $start_address $length
    fi

    copy_and_verify $file_path $partition
}

set +ex

main