#include <stddef.h>

#include "types.h"
#include "multiboot.h"
#include "asserts.h"
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
#include "sleep.h"
#include "KeyboardReader.h"
#include "FileSystem/RamDisk.h"
#include "syscall.h"
#include "Loader/loader.h"

#ifdef TESTS
#include "tests/tests.h"
#endif

 
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__) || !defined(__i386__)
#error "You must compile with a cross compiler for i386"
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
	kprintf("1: Heap space: %d\n", KernelHeapAllocator::the().current_free_space());
	char* b1;
	char* b2;
	b1 = alloc(0x20);
	b2 = alloc(0x20);
	kprintf("2: Heap space: %d\n", KernelHeapAllocator::the().current_free_space());
	delete[] b1;
	delete[] b2;
	// b1 = alloc(0x20);
	kprintf("3: Heap space: %d\n", KernelHeapAllocator::the().current_free_space());
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


void print_heap() {

	u32 num_blocks = 0;
	u32 bytes = KernelHeapAllocator::the().current_free_space(num_blocks);
	kprintf("heap free bytes: %d, #blocks: %d\n", num_blocks, bytes);

}

void test_usemode() {
	u32 elf_size = 0;
	u8* elf_data = RamDisk::fs().get_content("userspace/main", elf_size);
	ASSERT(elf_data != nullptr, "couldn't load userspace/main");
	load_and_jump_userspace(elf_data, elf_size);
}

void task1_func() {
	test_usemode();
	for(int i = 0; ; i++) {
		kprintf("task1: %d\n", i);
		sleep_ms(150);
	}
}

void task2_func() {
	test_usemode();
	for(int i = 0; ; i++) {
		kprintf("task2: %d\n", i);
		print_heap();
		sleep_ms(200);
	}
}
void task3_func() {
	test_usemode();
	for(int i = 0; i < 5; i++) {
		kprintf("task3: %d\n", i);
		sleep_ms(300);
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

void vga_tty_consumer() {
	while(1) {
		auto key_event = keyboard_read();
		if(!key_event.released && key_event.to_ascii() != 0) {
			VgaTTY::the().putchar(key_event.to_ascii());
		}
	}
}

void idle() {
	for(;;) {
		// kprintf("idle\n");
		// cpu_hang();
	}
}

void init_ramdisk(multiboot_info_t* mbt) {
	ASSERT(mbt->mods_count == 1, "Expected only a single multiboot module (ramdisk)");
	multiboot_module_t* ramdisk_module = (multiboot_module_t*) mbt->mods_addr;
	kprintf("mod_start: 0x%x, mod_end: 0x%x\n", ramdisk_module->mod_start, ramdisk_module->mod_end);
	void* ramdisk_base = (void*) ramdisk_module->mod_start;
	u32 ramdisk_size = ramdisk_module->mod_end - ramdisk_module->mod_start + 1;
	RamDisk::init(ramdisk_base, ramdisk_size);
}

#define HELLO_FILE "hello.txt"

void vga_tty_hello() {
	u32 size = 0;
	u8* content = RamDisk::fs().get_content(HELLO_FILE, size);
	ASSERT(content != nullptr && size > 0, "error reading hello file");
	VgaTTY::the().write((char*) content);
}


extern "C" void kernel_main(multiboot_info_t* mbt, unsigned int magic) {
	kprint("*******\nkernel_main\n*******\n\n");
	ASSERT(magic == MULTIBOOT_BOOTLOADER_MAGIC, "multiboot magic");
	kprintf("I smell %x\n", 0xdeadbeef);
	kprintf("Multiboot modules: mods_count: %d\n, mods_addr: 0x%x\n", mbt->mods_count, mbt->mods_addr);
	init_ramdisk(mbt);
	PIC::initialize();
	init_descriptor_tables();
	PIT::initialize();
	kmalloc_set_mode(KMallocMode::KMALLOC_ETERNAL);
	MemoryManager::initialize(mbt);
	KernelHeapAllocator::initialize();
	kmalloc_set_mode(KMallocMode::KMALLOC_NORMAL);


#ifdef TESTS
	run_tests();
	return;
#endif

	vga_tty_hello();
	PS2Keyboard::initialize();

	init_syscalls();
	// kprint("b4 int 0x80");
	// asm volatile ("int $0x80");
	// kprint("af int 0x80\n");
	// cpu_hang();
	Scheduler::initialize(idle);
	MemoryManager::the().lock_kernel_PDEs();
	Scheduler::the().add_task(create_kernel_task(task1_func, "task1"));
	Scheduler::the().add_task(create_kernel_task(task2_func, "task2"));
	Scheduler::the().add_task(create_kernel_task(task3_func, "task3"));
	Scheduler::the().add_task(create_kernel_task(vga_tty_consumer, "VgaTTY Consumer"));

	kprintf("enableing interrupts\n");
	asm volatile("sti");
	kprintf("enabled interrupts\n");



	// hang until scheduler ticks & switches to the idle task
	for(;;){}


	kprint("kernel_main end \n");
}
