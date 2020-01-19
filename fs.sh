#!/bin/bash


echo "hello1" > mnt/a.txt
echo "hello2" > mnt/b.txt
echo "hello2fsasf" > mnt/thef.dat
mkdir -p mnt/a
echo "sometext" > mnt/a/myfile.txt
mkdir -p mnt/a/subdir
echo "akjlfhasjkrfhsakjlfhaslfa" > mnt/a/subdir/another.dat
echo "1234" > mnt/a/averylongfilename123.blah
echo "abcd" > mnt/a/NameWithCaps.blah
mkdir -p mnt/dir2
echo "123" > mnt/dir2/aa.txt

# copy userspace binaries
USERSPACE_BINARIES=""
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/shell.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/HelloWorld.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/terminal.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/cat.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/vi.app"

mkdir -p mnt/bin
cp $USERSPACE_BINARIES mnt/bin

mkdir -p mnt/init
cp init_files/* mnt/init

# temp fixes
rm mnt/init/kernel_*
mv mnt/bin/HelloWorld.app mnt/bin/hello.app

sync
