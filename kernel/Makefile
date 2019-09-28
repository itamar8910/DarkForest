

LIBC_INCLUDE := ../libc/include
KERNEL_INCLUDE := include
INCLUDE := -I$(LIBC_INCLUDE) -I$(KERNEL_INCLUDE)
LIBC_BUILD := ../libc/build

BUILD := build

all: boot kernel VgaTTY VgaTextDriver
	$(CC) -T linker.ld -o $(BUILD)/myos.bin $(LDFLAGS) $(BUILD)/boot.o $(BUILD)/kernel.o $(LIBC_BUILD)/libc.o $(BUILD)/VgaTextDriver.o $(BUILD)/VgaTTY.o -lgcc

boot:
	$(ASM) -felf32 boot.S -o $(BUILD)/boot.o

kernel:
	$(CC) $(INCLUDE) -c kernel.cpp -o $(BUILD)/kernel.o $(CFLAGS)

VgaTTY:
	$(CC) $(INCLUDE) -c VgaTTY.cpp -o $(BUILD)/VgaTTY.o $(CFLAGS)

VgaTextDriver:
	$(CC) $(INCLUDE) -c VgaTextDriver.cpp -o $(BUILD)/VgaTextDriver.o $(CFLAGS)


clean:
	rm -f $(BUILD)/*