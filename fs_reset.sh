#!/bin/bash

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

# create FAT32 fs image for hard disk
umount mnt
rm -r mnt/
rm disk_img.bin
