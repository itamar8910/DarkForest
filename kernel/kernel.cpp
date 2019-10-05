#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "types.h"
#include "multiboot.h"
#include "assert.h"
#include "string.h"
#include "VgaTTY.h"
#include "DebugPort.h"
#include "cpu.h"
#include "logging.h"

 
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void do_vga_tty_stuff() {
	auto tty = VgaTTY();
 
	tty.write("up1\n");
	tty.write("up2\n");
	tty.write("up3\n");
	for(int i = 0; i < 20; i++) {
		tty.write("another line\n");
	}
	tty.write("down1\n");
	tty.write("down2\n");
	tty.write("down3fshakfa\n");
	tty.write("down4\n");
}

struct [[gnu::packed]] MultibootMemMapEntry {
	unsigned int size;
	unsigned int base_addr_low,base_addr_high;
// You can also use: unsigned long long int base_addr; if supported.
	unsigned int length_low,length_high;
// You can also use: unsigned long long int length; if supported.
	unsigned int type;
};

void init_memory(multiboot_info_t* mbt) {
	// get physical memory map
	// get first entry in mmap list
	kprintf("mmap addr: 0x%x\n", mbt->mmap_addr);
	MultibootMemMapEntry* mmap = (MultibootMemMapEntry*) mbt->mmap_addr;
	while((uint32_t) mmap < mbt->mmap_addr + mbt->mmap_length) {
		// ASSERT(mmap->base_addr_high == 0, "high=0");
		// ASSERT(mmap->length_high == 0, "high=0");
		u32 start = mmap->base_addr_low;
		u32 end = start + mmap->length_low;
		kprintf("type: %d, start: 0x%x, len: : 0x%x, end: 0x%x\n", mmap->type, start, mmap->length_low, end);
		kprintf("mmap size: %x\n", mmap->size);
		mmap = (MultibootMemMapEntry*) ( (unsigned int)mmap + mmap->size + sizeof(mmap->size) );
	}
}

extern "C" void kernel_main(multiboot_info_t* mbt, unsigned int magic) {
	kprint("kernel_main\n");
	kprintf("I smell %x\n", 0xdeadbeef);
	init_descriptor_tables();
	kprintf("0x%x\n", (unsigned int) 258);
	ASSERT(magic == MULTIBOOT_BOOTLOADER_MAGIC, "multiboot magic");
	init_memory(mbt);
	do_vga_tty_stuff();
	int val = 256;
	// kprintf("%s\n", "hello");
	kprintf("%d %s 0x%x %s\n", val, "in decimal is", val, "in hex");
	kprint("kernel_main end \n");
}
