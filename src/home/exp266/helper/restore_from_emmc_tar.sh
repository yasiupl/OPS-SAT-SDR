#!/usr/bin/env sh
set +x

dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832 | gnu_tar.tar xv -C /tmp 
ls -lah /tmp

set -x