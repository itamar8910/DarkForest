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
#include "MemoryManager.h"

 
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



extern "C" void kernel_main(multiboot_info_t* mbt, unsigned int magic) {
	kprint("kernel_main\n");
	kprintf("I smell %x\n", 0xdeadbeef);
	init_descriptor_tables();
	ASSERT(magic == MULTIBOOT_BOOTLOADER_MAGIC, "multiboot magic");
	MemoryManager::initialize(mbt);
	do_vga_tty_stuff();
	kprint("kernel_main end \n");
}
