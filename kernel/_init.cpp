#include <stddef.h>

#include "types.h"
#include "multiboot.h"
#include "asserts.h"
#include "string.h"
#include "VgaTTY.h"
#include "drivers/DebugPort.h"
#include "cpu.h"
#include "logging.h"
#include "flags.h"
#include "MM/MemoryManager.h"
#include "MM/MM_types.h"
#include "kmalloc.h"
#include "task.h"
#include "drivers/PIC.h"
#include "drivers/PIT.h"
#include "Scheduler.h"
#include "drivers/PS2Keyboard.h"
#include "sleep.h"
#include "FileSystem/RamDisk.h"
#include "syscall.h"
#include "Loader/loader.h"
#include "FileSystem/VFS.h"
#include "FileSystem/DevFS.h"
#include "HAL/KeyboardDevice.h"
#include "FileSystem/CharFile.h"
#include "FileSystem/FileUtils.h"

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
	MemoryManager::the().allocate(addr, PageWritable::YES, UserAllowed::YES);
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

void hello_world_userspace() {
	size_t elf_size = 0;
	File* f = VFS::the().open("/initrd/userspace/HelloWorld.app");
	ASSERT(f != nullptr, "couldn't open HelloWorld");
	u8* elf_data = FileUtils::read_all(*static_cast<CharFile*>(f), elf_size);
	ASSERT(elf_data != nullptr, "couldn't load HelloWorld");
	load_and_jump_userspace(elf_data, elf_size);
	delete elf_data;
}
void vga_tty_userspace() {
	size_t elf_size = 0;
	File* f = VFS::the().open("/initrd/userspace/VgaTTY.app");
	ASSERT(f != nullptr, "couldn't open VgaTTY app");
	u8* elf_data = FileUtils::read_all(*static_cast<CharFile*>(f), elf_size);
	ASSERT(elf_data != nullptr, "couldn't load VgaTTY app");
	load_and_jump_userspace(elf_data, elf_size);
}



void vga_tty_consumer() {
	KeyboardDevice* kbd = (KeyboardDevice*) VFS::the().open("/dev/keyboard");
	ASSERT(kbd != nullptr, "failed to open keyboard device");
	while(1) {
		KeyEvent key_event;
		int rc = kbd->read(1, &key_event);
		ASSERT(rc==1, "failed to read from keyboard");
		if(!key_event.released && key_event.to_ascii() != 0) {
			VgaTTY::the().putchar(key_event.to_ascii());
		}
	}
	delete kbd;
}

void idle() {
	for(;;) {
		// kprintf("idle\n");
		// cpu_hang();
	}
}

// #define HELLO_FILE "hello.txt"
// 
// void vga_tty_hello() {
// 	u32 size = 0;
// 	u8* content = RamDisk::fs().get_content(HELLO_FILE, size);
// 	ASSERT(content != nullptr && size > 0, "error reading hello file");
// 	VgaTTY::the().write((char*) content);
// }

void init_VFS() {
	VFS::the().mount(&DevFS::the());
	VFS::the().mount(&RamDisk::fs());
}

extern "C" void kernel_main(multiboot_info_t* mbt, unsigned int magic) {
	kprint("*******\nkernel_main\n*******\n\n");
	ASSERT(magic == MULTIBOOT_BOOTLOADER_MAGIC, "multiboot magic");
	kprintf("I smell %x\n", 0xdeadbeef);
	kprintf("Multiboot modules: mods_count: %d\n, mods_addr: 0x%x\n", mbt->mods_count, mbt->mods_addr);
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

	PS2Keyboard::initialize();

	RamDisk::init(*mbt);
	DevFS::initiailize();
	init_VFS();

	// vga_tty_hello();

	init_syscalls();
	Scheduler::initialize(idle);
	MemoryManager::the().lock_kernel_PDEs();
	Scheduler::the().add_process(Process::create(hello_world_userspace, "HelloWorldUser"));
	Scheduler::the().add_process(Process::create(vga_tty_userspace, "VgaTTYUser"));
	Scheduler::the().add_process(Process::create(vga_tty_consumer, "VgaTTY"));

	kprintf("enableing interrupts\n");
	asm volatile("sti");
	kprintf("enabled interrupts\n");



	// hang until scheduler ticks & switches to the idle task
	for(;;){}


	kprint("kernel_main end \n");
}
