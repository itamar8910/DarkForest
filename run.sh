#!/bin/bash

qemu=qemu-system-i386
image=kernel/DarkForest.bin
memory=100M

$qemu -kernel $image \
    -debugcon stdio\
    -m $memory \
    -d guest_errors \
    -initrd ramdisk/ramdisk.tar \
    -hda disk_img.bin

