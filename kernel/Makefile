

ASM = nasm
CC = i686-elf-gcc
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra

all: boot kernel
	$(CC) -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

boot:
	$(ASM) -felf32 boot.S -o boot.o

kernel:
	$(CC) -c kernel.c -o kernel.o $(CFLAGS)
