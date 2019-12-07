#!/bin/bash

# copy userspace binaries

USERSPACE_BINARIES=""
USERSPACE_BINARIES="$USERSPACE_BINARIES ../userspace/shell.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES ../userspace/HelloWorld.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES ../userspace/terminal.app"
USERSPACE_BINARIES="$USERSPACE_BINARIES ../userspace/cat.app"

pushd ramdisk

if [ -f ramdisk.tar ]; then
rm ramdisk.tar
fi
tar --format=v7 -cf ramdisk.tar * $USERSPACE_BINARIES

popd