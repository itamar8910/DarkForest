//
// isr.c -- High level interrupt service routines and interrupt request handlers.
// Part of this code is modified from Bran's kernel development tutorials.
// Rewritten for JamesM's kernel development tutorials.
//

#include "isr.h"
#include "DebugPort.h"

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

void aaa(char* data) {
   outb(0xe9 ,'Y');
   DebugPort::write("recieved interrupt: \n");
}

// This gets called from our ASM interrupt handler stub.
extern "C" void isr_handler(char* data)
{
   outb(0xe9 ,'X');
   aaa(data);
}

/*
void insr_handle_cpp(isr_saved_registers regs) {
   outb(0xe9 ,'Y');
   DebugPort::write("recieved interrupt: ");
}
// This gets called from our ASM interrupt handler stub.
extern "C" void isr_handler(isr_saved_registers regs)
{
   outb(0xe9 ,'X');
   insr_handle_cpp(regs);
   // DebugPort::write("recieved interrupt: ");
   // DebugPort::write_dec(regs.int_no);
   // DebugPort::write("\n");
}
*/