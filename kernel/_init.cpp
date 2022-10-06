#include <stddef.h>

#include "types.h"
#include "multiboot.h"
#include "asserts.h"
#include "cstring.h"
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
#include "syscall.h"
#include "Loader/loader.h"
#include "FileSystem/VFS.h"
#include "FileSystem/DevFS.h"
#include "HAL/KeyboardDevice.h"
#include "FileSystem/CharFile.h"
#include "FileSystem/FileUtils.h"
#include "kernel_symbols.h"
#include "shared_ptr.h"
#include "HAL/VgaTTY.h"
#include "drivers/ATADisk.h"
#include "FileSystem/Fat32FS.h"
#include "lock.h"
#include "FileSystem/PtsFS.h"
#include "drivers/PS2Mouse.h"
#include "drivers/PCIBus.h"
#include "drivers/RTL8139NetworkCard.h"
#include "Network/Arp.h"
#include "Network/NetworkManager.h"

#ifdef TESTS
#include "tests/tests.h"
#endif

 
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__) || !defined(__i386__)
#error "You must compile with a cross compiler for i386"
#endif
 

void hello_world_userspace() {
	load_and_jump_userspace("/bin/HelloWorld.app");
}

void terminal_userspace() {
	load_and_jump_userspace("/bin/terminal.app");
}

void start_windowserver() {
	load_and_jump_userspace("/bin/WindowServer.app");
}

void start_gui_terminal() {
	load_and_jump_userspace("/bin/GuiTerminal.app");
}

void start_gui2() {
	load_and_jump_userspace("/bin/gui2.app");
}

void idle() {
	for(;;) {
		// kprintf("idle\n");
		// cpu_hang();
	}
}

void init_VFS() {
	VFS::the().mount(&DevFS::the());
	VFS::the().mount(&Fat32FS::the());
	VFS::the().mount(&PtsFS::the());
}

void init_kernel_symbols() {
	#ifdef KERNEL_SYMBOLS_ENABLED
	KernelSymbols::initialize();
	#endif
}

volatile int glob_a = 0;

Lock& get_test_lock()
{
	static Lock lock("TestLock");
	return lock;
}    

void network_task()
{
	sleep_ms(1000);
	kprintf("***********************\n********************\n");
	kprintf("network_task transmitting\n");

	u8 data[] = "\xff\xff\xff\xff\xff\xff\x7a\x50\x8d\x2c\x77\x5c\x08\x06\x00\x01" \
"\x08\x00\x06\x04\x00\x01\x7a\x50\x8d\x2c\x77\x5d\xc0\xa8\x02\x14" \
"\xff\xff\xff\xff\xff\xff\xc0\xa8\x02\x01\x00\x00\x00\x00\x00\x00" \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	(void)data;

	kprintf("transmit1\n");
	Network::Arp::send_arp_request(Network::IPV4 {192,168,2,1}, Network::IPV4 {192,168,2,20});
	// RTL8139NetworkCard::the().transmit(data, sizeof(data));
	kprintf("transmit2\n");
	Network::Arp::send_arp_request(Network::IPV4 {192,168,2,1}, Network::IPV4 {192,168,2,20});
	kprintf("transmit3\n");
	Network::Arp::send_arp_request(Network::IPV4 {192,168,2,1}, Network::IPV4 {192,168,2,20});
	kprintf("transmit4\n");
	Network::Arp::send_arp_request(Network::IPV4 {192,168,2,1}, Network::IPV4 {192,168,2,20});
	kprintf("transmit5\n");
	Network::Arp::send_arp_request(Network::IPV4 {192,168,2,1}, Network::IPV4 {192,168,2,20});
	kprintf("transmit6\n");
	Network::Arp::send_arp_request(Network::IPV4 {192,168,2,1}, Network::IPV4 {192,168,2,20});
}

void task1()
{
	int N = 10;
	for(int i = 0; i < N; ++i)
	{
		// kprintf("task1\n");
		for(int i = 0; i < N; ++i)
		{
		// for(int i = 0; i < N; ++i)
		// {
		LOCKER(get_test_lock());
			glob_a+=1;
			sleep_ms(1);
			glob_a -= 1;
		// }
		}
	}
	kprintf("task1: %d\n", glob_a);
}

void task2()
{
	int N = 10;
	for(int i = 0; i < N; ++i)
	{
		// kprintf("task1\n");
		for(int i = 0; i < N; ++i)
		{
		// for(int i = 0; i < N; ++i)
		// {
		LOCKER(get_test_lock());
			glob_a+=1;
			sleep_ms(1);
			glob_a -= 1;
		// }
		}
	}
	kprintf("task2: %d\n", glob_a);
}

extern "C" void kernel_main(multiboot_info_t* mbt, unsigned int magic) {
	kprint("*******\nkernel_main\n*******\n\n");
	VgaTTY::the().write("DarKForest booting...\n");
	ASSERT(magic == MULTIBOOT_BOOTLOADER_MAGIC);
	kprintf("I smell %x\n", 0xdeadbeef);
	kprintf("Multiboot modules: mods_count: %d\n, mods_addr: 0x%x\n", mbt->mods_count, mbt->mods_addr);
	VgaTTY::the().write("Initializing PIC devices...\n");
	PIC::initialize();
	init_descriptor_tables();
	PIT::initialize();
	MemoryManager::initialize(mbt);
	KernelHeapAllocator::initialize();

	PS2Keyboard::initialize();
	PS2Mouse::initialize();
	ATADisk::initialize();

	kprintf("Initializing PCI\n");
	PCIBus::initialize();

	RTL8139NetworkCard::initialize();
	Network::NetworkManager::initialize(RTL8139NetworkCard::the().mac());
	RTL8139NetworkCard::the().enable_receive_transmit();

	VgaTTY::the().write("Initializing File Systems...\n");
	DevFS::initiailize();
	Fat32FS::initialize();
	// cpu_hang();

	init_VFS();

	VgaTTY::the().write("Loading kernel symbols...\n");
	init_kernel_symbols();

#ifdef TESTS
	run_tests();
	return;
#endif

	init_syscalls();
	VgaTTY::the().write("Initizliaing the Scheduler...\n");
	Scheduler::initialize(idle);
	MemoryManager::the().lock_kernel_PDEs();

	// Scheduler::the().add_process(Process::create(hello_world_userspace, "HelloWorldUser"));
	// Scheduler::the().add_process(Process::create(terminal_userspace, "TerminalUser"));
    Scheduler::the().add_process(Process::create(start_windowserver, "WindowServer"));
	Scheduler::the().add_process(Process::create(start_gui_terminal, "GuiTerminal"));
	// Scheduler::the().add_process(Process::create(start_gui2, "gui2"));

	// VGA::init();

	Scheduler::the().add_process(Process::create(network_task, "network_task"));
	// Scheduler::the().add_process(Process::create(task2, "task2"));

	kprintf("enableing interrupts\n");
	asm volatile("sti");
	kprintf("enabled interrupts\n");

	// hang until scheduler ticks & switches to the idle task
	for(;;){}


	kprint("kernel_main end \n");
}
