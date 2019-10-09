#include <stddef.h>

#include "types.h"
#include "multiboot.h"
#include "assert.h"
#include "string.h"
#include "VgaTTY.h"
#include "DebugPort.h"
#include "cpu.h"
#include "logging.h"
#include "MemoryManager.h"
#include "flags.h"

#ifdef TESTS
#include "tests.h"
#endif

 
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
	ASSERT(magic == MULTIBOOT_BOOTLOADER_MAGIC, "multiboot magic");
	kprintf("I smell %x\n", 0xdeadbeef);
	init_descriptor_tables();
	MemoryManager::initialize(mbt);
	kprintf("frame available: %d\n", MemoryManager::the().is_frame_available(0x000800000 + PAGE_SIZE));
	do_vga_tty_stuff();

#ifdef TESTS
	run_tests();
	return;
#endif

	// testing frame allocation
	Err err;
	for(int i = 0; i < 10; i++) {
		auto addr = MemoryManager::the().get_free_frame(err);
		MemoryManager::the().set_frame_used(addr);
		kprintf("frame: 0x%x, err:%d\n", addr, err);
	}
	kprintf("---\n");
	MemoryManager::the().set_frame_available(0x503000);
	for(int i = 0; i < 10; i++) {
		auto addr = MemoryManager::the().get_free_frame(err);
		MemoryManager::the().set_frame_used(addr);
		kprintf("frame: 0x%x, err:%d\n", addr, err);
	}

	char* buff = new char[50];
	kprintf("buff addr: 0x%x\n", buff);
	buff[0] = 'a';
	buff[1] = 'b';
	buff[2] = 'c';
	buff[3] = 0;
	kprintf("buff: %s\n", buff);
	buff = new char[100];
	kprintf("buff addr: 0x%x\n", buff);
	buff = new char[100];
	kprintf("buff addr: 0x%x\n", buff);
	kprint("kernel_main end \n");
}
