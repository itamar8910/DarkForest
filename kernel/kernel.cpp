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
	u64 base;
	u64 len;
	unsigned int type;
};
// struct [[gnu::packed]] MultibootMemMapEntry {
// 	unsigned int size;
// 	unsigned int base_low, base_high;
// 	unsigned int len_low, len_high;
// 	unsigned int type;
// };
static_assert(sizeof(MultibootMemMapEntry)==24);

void init_memory(multiboot_info_t* mbt) {
	// get physical memory map
	// get first entry in mmap list
	kprintf("mmap addr: 0x%x\n", mbt->mmap_addr);
	MultibootMemMapEntry* mmap = (MultibootMemMapEntry*) mbt->mmap_addr;
	while((uint32_t) mmap < mbt->mmap_addr + mbt->mmap_length) {
		// ASSERT(mmap->base_addr_high == 0, "high=0");
		// ASSERT(mmap->length_high == 0, "high=0");
		
		kprintf("type: %d\n", mmap->type);
		kprintf("base low: %x\n", mmap->base & 0xffffffff);
		kprintf("base high: %x\n", mmap->base >> 32 & 0xffffffff);
		kprintf("len low: %x\n", mmap->len & 0xffffffff);
		kprintf("len high : %x\n", mmap->len >> 32 & 0xffffffff);
		kprintf("----\n");
		// kprintf("Mem chunk type: %d, base: 0x%x|%x, len: 0x%x|%x\n",
		// 			mmap->type, 
		// 			mmap->base&0xffffffff,
		// 			mmap->base>>32,
		// 			mmap->length&0xffffffff,
		// 			mmap->length>>32);
		// u32 start = mmap->base;
		// u32 end = start + mmap->length_low;
		// kprintf("type: %d, start: 0x%x, len: : 0x%x, end: 0x%x\n", mmap->type, start, mmap->length_low, end);
		// kprintf("mmap size: %x\n", mmap->size);
		mmap = (MultibootMemMapEntry*) ( (unsigned int)mmap + mmap->size + sizeof(mmap->size) );
	}
}

extern "C" void kernel_main(multiboot_info_t* mbt, unsigned int magic) {
	(void)mbt;
	(void)magic;
	kprint("kernel_main\n");
	kprintf("hello! %015x\n", 842);
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
