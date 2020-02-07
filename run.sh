#!/bin/bash

qemu=qemu-system-i386
image=kernel/DarkForest.bin
memory=100M

$qemu -kernel $image \
    -debugcon stdio\
    -m $memory \
    -d guest_errors \
    -hda disk_img.bin \
    -device VGA,vgamem_mb=64

