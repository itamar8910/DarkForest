#!/bin/bash

# create FAT32 fs image for hard disk

qemu-img create disk_img.bin 600m
chown 1001:1001 disk_img.bin
mkfs.vfat -F 32 disk_img.bin
mkdir -p mnt
mount disk_img.bin mnt -o uid=1001 -o gid=1001

echo "hello1" > mnt/a.txt
echo "hello2" > mnt/b.txt
echo "hello2fsasf" > mnt/thef.dat
mkdir -p mnt/a
echo "sometext" > mnt/a/myfile.txt
mkdir -p mnt/a/subdir
echo "akjlfhasjkrfhsakjlfhaslfa" > mnt/a/subdir/another.dat
mkdir -p mnt/dir2
echo "123" > mnt/dir2/aa.txt

sync
