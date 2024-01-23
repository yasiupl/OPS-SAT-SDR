#!/usr/bin/env sh
#set -x

## SDR-compatible Firmware:
#0x00000223
#0x00000202
firmware=$(devmem 0xff200000)
force_reload=${1:-"false"}

echo "Loaded firmware: $firmware"

if [[ $firmware == "0x00000223" ]] || [[ $firmware == "0x00000202" ]]; then
    echo "Correct firmware loaded (202/223)"
else
    echo "FPGA Firmware not loaded - loading will cause reboot;"

    if [[ "$force_reload" == "force_reload" ]]; then
        echo "Forcing install of FPGA firmware! SEPP will reboot any moment now!"
    else
        echo "Refusing reboot. Stopping now."
        exit 11
    fi
fi


cd /tmp
## Exp202 software (device tree)
# dd if=/dev/mmcblk0 bs=512 skip=13664256 count=16384 | gnu_tar.tar xv -C /tmp 
# opkg install exp202_6.0_sepp.ipk
## EXP223 firmware
echo "Reapplying device-tree"
dd if=/dev/mmcblk0 bs=512 skip=14745600 count=16384 | gnu_tar.tar xv -C /tmp
opkg install exp223-firmware_1.3_sepp.ipk
cd /home/exp5223/
## If firmware is not loaded, it will reboot here, better check before.
./start_exp5223.sh

#set +x

exit 0