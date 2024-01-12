#!/bin/sh

set -ex

# Instalation check #
opkg status exp266

# Check Config modification
cat /home/exp266/config.ini

ls -Rlh /home/exp266/

tail -n +1 /home/exp266/toGround/*/*.log

set +ex
