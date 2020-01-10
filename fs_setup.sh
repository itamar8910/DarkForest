#!/bin/bash

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

# create FAT32 fs image for hard disk
qemu-img create disk_img.bin 600m
chown 1001:1001 disk_img.bin
mkfs.vfat -F 32 disk_img.bin
mkdir -p mnt
mount disk_img.bin mnt -o uid=1001 -o gid=1001