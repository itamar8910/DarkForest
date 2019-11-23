#!/bin/bash

# copy userspace binaries

USERSPACE_BINARIES="../userspace/VgaTTY.app ../userspace/HelloWorld.app"

pushd ramdisk

if [ -f ramdisk.tar ]; then
rm ramdisk.tar
fi
tar --format=v7 -cf ramdisk.tar * $USERSPACE_BINARIES

popd