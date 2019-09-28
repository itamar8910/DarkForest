

INCLUDE := -I../libc/include
LIBC_BUILD := ../libc/build

BUILD := build

all: boot kernel
	$(CC) -T linker.ld -o $(BUILD)/myos.bin $(LDFLAGS) $(BUILD)/boot.o $(BUILD)/kernel.o $(LIBC_BUILD)/libc.o -lgcc

boot:
	$(ASM) -felf32 boot.S -o $(BUILD)/boot.o

kernel:
	$(CC) $(INCLUDE) -c kernel.cpp -o $(BUILD)/kernel.o $(CFLAGS)

clean:
	rm -f $(BUILD)/*