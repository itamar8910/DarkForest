#include <stddef.h>

#include "types.h"
#include "multiboot.h"
#include "asserts.h"
#include "string.h"
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
#include "kernel_symbols.h"

#ifdef TESTS
#include "tests/tests.h"
#endif

 
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__) || !defined(__i386__)
#error "You must compile with a cross compiler for i386"
#endif
 

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

void idle() {
	for(;;) {
		// kprintf("idle\n");
		// cpu_hang();
	}
}

void init_VFS() {
	VFS::the().mount(&DevFS::the());
	VFS::the().mount(&RamDisk::fs());
}

void init_kernel_symbols() {
	KernelSymbols::initialize();
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

	init_kernel_symbols();

	init_syscalls();
	Scheduler::initialize(idle);
	MemoryManager::the().lock_kernel_PDEs();
	Scheduler::the().add_process(Process::create(hello_world_userspace, "HelloWorldUser"));
	Scheduler::the().add_process(Process::create(vga_tty_userspace, "VgaTTYUser"));

	// VFS::the().open("/inird/helllo.txt");
	// XASSERT(false);

	kprintf("enableing interrupts\n");
	asm volatile("sti");
	kprintf("enabled interrupts\n");

	// hang until scheduler ticks & switches to the idle task
	for(;;){}


	kprint("kernel_main end \n");
}
