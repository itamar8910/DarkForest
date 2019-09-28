#!/bin/bash

qemu=qemu-system-i386
image=kernel/myos.bin

$qemu -kernel $image
