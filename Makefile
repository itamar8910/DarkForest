KERNEL_DIR := kernel
LIBC_DIR := libc

export ASM := nasm
export CC := i686-elf-g++
export LD = i686-elf-ld
export CFLAGS := -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -mno-red-zone

export LDFLAGS := -ffreestanding -O2 -nostdlib

.PHONY: all libc kernel

all: libc kernel

libc:
	make -C $(LIBC_DIR)/

kernel:
	make -C $(KERNEL_DIR)/

clean:
	make -C $(KERNEL_DIR)/ clean
	make -C $(LIBC_DIR)/ clean