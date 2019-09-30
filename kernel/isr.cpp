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


// This gets called from our ASM interrupt handler stub.
// Note: although the registers state itself is on the stack,
// we pass by regerence instead of by value
// because otherwise g++ optimization could overwrite the regs argument
// (this took a couple of hours to debug haha)
extern "C" void isr_handler(isr_saved_registers& regs)
{
   outb(0xe9 ,'X');
   DebugPort::write("recieved interrupt: \n");
   char buff[3];
   buff[0] = '0' + regs.int_no;
   buff[1] = '\n';
   buff[2] = 0;
   DebugPort::write(buff);

}
