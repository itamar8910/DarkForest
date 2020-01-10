#!/bin/bash

qemu=qemu-system-i386
image=kernel/DarkForest.bin

$qemu -kernel $image \
    -debugcon stdio\
    -m 1G \
    -d guest_errors \
    -hda disk_img.bin


