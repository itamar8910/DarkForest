#!/bin/bash

qemu=qemu-system-i386
image=kernel/build/myos.bin

$qemu -kernel $image
