#!/bin/bash

# create FAT32 fs image for hard disk

#!/bin/bash
qemu-img create disk_img.bin 600m
mkfs.vfat -F 32 disk_img.bin
mkdir mnt
mount disk_img.bin mnt -o uid=1001 -o gid=1001
