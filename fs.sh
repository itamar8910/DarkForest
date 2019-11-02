#!/bin/bash

pushd ramdisk

if [ -f ramdisk.tar ]; then
rm ramdisk.tar
fi
tar --format=v7 -cf ramdisk.tar *

popd