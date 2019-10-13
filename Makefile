KERNEL_DIR := kernel
LIBC_DIR := libc

export ASM := nasm
export CC := i686-elf-g++
export LD = i686-elf-ld
export CFLAGS := --std=c++17 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -mno-red-zone -fno-sized-deallocation

export LDFLAGS := -ffreestanding -O2 -nostdlib

export CPP_FLAGS := 

.PHONY: all libc kernel

all: libc kernel

tests: clean set_tests_make_cmd all

set_tests_make_cmd:
	$(eval CPP_FLAGS := -DTESTS)

libc:
	make -C $(LIBC_DIR)/

kernel:
	make -C $(KERNEL_DIR)/

clean:
	make -C $(KERNEL_DIR)/ clean
	make -C $(LIBC_DIR)/ clean