#!/bin/bash


mkdir -p mnt/stuff
if [[ $? -ne 0 ]]
then
    echo "failed to prepare fs, run fs_scripts/fs_reset.sh & then fs_scripts/fs_setup.sh"
    exit 1
fi
echo "hello1" > mnt/stuff/a.txt
echo "hello2" > mnt/stuff/b.txt
mkdir -p mnt/stuff/a
echo "1234" > mnt/stuff/a/averylongfilename123.blah
echo "abcd" > mnt/stuff/a/NameWithCaps.blah

# copy userspace binaries
USERSPACE_BINARIES=""
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/shell.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/HelloWorld.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/TextTerminal.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/cat.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/vi.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/ls.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/echo.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/touch.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/mkdir.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/WindowServer.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/GuiTerminal.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES userspace/gui2.app"

mkdir -p mnt/bin
cp $USERSPACE_BINARIES mnt/bin

mkdir -p mnt/init
cp init_files/* mnt/init

# temp fixes
rm mnt/init/kernel_*

sync
