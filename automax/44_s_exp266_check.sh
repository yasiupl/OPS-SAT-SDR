#!/bin/sh

set -ex

# Instalation check #
opkg status exp266

# Check Config modification
cat /home/exp266/config.ini

ls -Rlh /home/exp266/

tail -n +1 /home/exp266/toGround/*/*.log

dd if=/dev/mmcblk0 bs=512 skip=13680640 count=376832 | gnu_tar.tar tv

set +ex
