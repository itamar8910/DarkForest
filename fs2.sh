#!/bin/bash

# create FAT32 fs image for hard disk

#!/bin/bash
qemu-img create disk_img.bin 600m
mkfs.vfat -F 32 disk_img.bin
mkdir mnt
mount disk_img.bin mnt
echo "hello1" > mnt/a.txt
echo "hello2" > mnt/b.txt
echo "hello2fsasf" > mnt/thef.dat
umount mnt
rm -r mnt
