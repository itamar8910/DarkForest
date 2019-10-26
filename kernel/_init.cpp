#include <stddef.h>

#include "types.h"
#include "multiboot.h"
#include "Kassert.h"
#include "string.h"
#include "VgaTTY.h"
#include "DebugPort.h"
#include "cpu.h"
#include "logging.h"
#include "flags.h"
#include "MM/MemoryManager.h"
#include "MM/MM_types.h"
#include "kmalloc.h"
#include "task.h"
#include "PIC.h"
#include "PIT.h"
#include "Scheduler.h"
#include "PS2Keyboard.h"

#ifdef TESTS
#include "tests/tests.h"
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
	auto tty = VgaTTY::the();
 
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

char* alloc(int size) {
	char* buff = new char[size];
	memset(buff, 0, size);
	kprintf("buff addr: 0x%x\n", buff);
	return buff;
}

void try_malloc() {
	kprintf("1: Heap space: %d\n", KMalloc::the().current_free_space());
	char* b1;
	char* b2;
	b1 = alloc(0x20);
	b2 = alloc(0x20);
	kprintf("2: Heap space: %d\n", KMalloc::the().current_free_space());
	delete[] b1;
	delete[] b2;
	// b1 = alloc(0x20);
	kprintf("3: Heap space: %d\n", KMalloc::the().current_free_space());
	b1 = alloc(0x2000);
	delete[] b1;
	alloc(0x200);
}

void try_frame_alloc() {
	// testing frame allocation
	Err err;
	for(int i = 0; i < 10; i++) {
		auto addr = MemoryManager::the().get_free_frame(err);
		kprintf("frame: 0x%x, err:%d\n", addr, err);
	}
	kprintf("---\n");
	MemoryManager::the().set_frame_available(0x503000);
	for(int i = 0; i < 10; i++) {
		auto addr = MemoryManager::the().get_free_frame(err);
		kprintf("frame: 0x%x, err:%d\n", addr, err);
	}
}

void try_virtual_alloc() {
	u32 addr = 0x80000000;
	MemoryManager::the().allocate(addr, true, true);
	char* str = (char*)addr;
	strncpy(str, "I am allocated", 100);
	str[4*1024 ] = 1; // this will generate a page fault

	kprintf("%s\n", str);


}



void task1_func() {
	for(int i = 0; ; i++) {
		kprintf("task1: %d\n", i);
		Scheduler::the().sleep_ms(150);
	}
}
void task2_func() {
	for(int i = 0; ; i++) {
		kprintf("task2: %d\n", i);
		Scheduler::the().sleep_ms(200);
	}
}
void task3_func() {
	for(int i = 0; i < 5; i++) {
		kprintf("task3: %d\n", i);
		Scheduler::the().sleep_ms(300);
	}
}


void try_count_seconds() {
	int x = 9999;
	while(1) {
		int y = PIT::seconds_since_boot();
		if(x != y) {
			x = y;
			kprintf("Seconds: %d\n", x);
		}
	}
}

void idle() {
	for(;;) {
		// kprintf("idle\n");
		// cpu_hang();
	}
}

extern "C" void kernel_main(multiboot_info_t* mbt, unsigned int magic) {
	kprint("*******\nkernel_main\n*******\n\n");
	ASSERT(magic == MULTIBOOT_BOOTLOADER_MAGIC, "multiboot magic");
	kprintf("I smell %x\n", 0xdeadbeef);
	PIC::initialize();
	init_descriptor_tables();
	PIT::initialize();
	kmalloc_set_mode(KMallocMode::KMALLOC_ETERNAL);
	MemoryManager::initialize(mbt);
	KMalloc::initialize();
	kmalloc_set_mode(KMallocMode::KMALLOC_NORMAL);

	PS2Keyboard::initialize();

#ifdef TESTS
	run_tests();
	return;
#endif

	// VgaTTY::the()
	// do_vga_tty_stuff();


	Scheduler::initialize(idle);
	MemoryManager::the().lock_kernel_PDEs();
	Scheduler::the().add_task(create_kernel_task(task1_func, "task1"));
	Scheduler::the().add_task(create_kernel_task(task2_func, "task2"));
	Scheduler::the().add_task(create_kernel_task(task3_func, "task3"));

	kprintf("enableing interrupts\n");
	asm volatile("sti");
	kprintf("enabled interrupts\n");


	// hang until scheduler ticks & switches to the idle task
	for(;;){}


	kprint("kernel_main end \n");
}
