#!/bin/sh

set -ex

# Installing #
opkg install /esoc-apps-flash/fms/filestore/exp266_*.ipk

# Instalation check #
opkg status exp266

# Check Config modification
cat /home/exp266/config.ini

ls -Rlh /home/exp266/

cat /home/exp266/entrypoint.sh

set +ex