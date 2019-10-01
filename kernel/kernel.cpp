#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "Cstring.h"

#include "VgaTTY.h"
#include "DebugPort.h"
#include "cpu_descriptors.h"
 
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

extern "C" void kernel_main(void) 
{
	DebugPort::write("kernel_main\n");
   init_descriptor_tables();
   // test ISRs
   asm volatile ("int $0x3");
   asm volatile ("int $0x3");
//    asm volatile ("int $0x4"); 
   do_vga_tty_stuff();
	DebugPort::write("kernel_main end \n");
}
