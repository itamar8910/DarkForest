#!/bin/bash

is_nt=false

if [ "$(expr substr $(uname -s) 1 7)" == "MSYS_NT" ]; then
    is_nt=true
fi

if [ "$is_nt" == true ]; then
    r_partition_dev_name=$(grep R: /proc/partitions | awk '{print $4}')
    dd if=/dev/$r_partition_dev_name of=disk_img.bin status=progress bs=1M
fi

if [[ "$1" == "debug" ]]; then
    debug="-s -S"
    echo "Starting in debug mode, waiting for gdb to connect on port 1234"
fi

qemu=qemu-system-i386
image=kernel/DarkForest.bin
memory=100M

$qemu -kernel $image \
    -debugcon stdio\
    -m $memory \
    -d guest_errors \
    -hda disk_img.bin \
    -device VGA,vgamem_mb=64 $debug \
    -object filter-dump,id=hue,netdev=br0,file=rtl8139.pcap \
    -netdev tap,ifname=tap0,id=br0 -device rtl8139,netdev=br0
