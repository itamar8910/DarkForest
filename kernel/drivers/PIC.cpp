
// based on: 
// - https://wiki.osdev.org/8259_PIC#What_does_the_8259_PIC_do.3F
// - serenity OS

#include "PIC.h"
#include "IO.h"
#include "logging.h"

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI		0x20		/* End-of-interrupt command code */

namespace PIC {
    void initialize() {
        kprintf("PIC::initialize\n");
        // start initialization, cascade (master & slave) mode
        IO::outb(PIC1_COMMAND, 0x11, true);
        IO::outb(PIC2_COMMAND, 0x11, true);

        // set IRQs base in ISR array
        IO::outb(PIC1_DATA, IRQ_ISRS_BASE, true); // master 
        IO::outb(PIC2_DATA, IRQ_ISRS_BASE + 8, true); // slave

        // configure master/slave
        IO::outb(PIC1_DATA, 4, true); // tell master there's a slave at index 2 (=1 << 2)
        IO::outb(PIC2_DATA, 2, true); // tell slave

        // x86 mode
        IO::outb(PIC1_DATA, 0x1);
        IO::outb(PIC2_DATA, 0x1);

        // start our with all IRQs masked (disabled)
        IO::outb(PIC1_DATA, 0xff);
        IO::outb(PIC2_DATA, 0xff);

        // enable IRQ2 (needed for master to forward slave IRQS)
        PIC::enable_irq(2);
    }
    void eoi(u8 irq) { // signal end of interrupt
        if(irq >= 8)
            IO::outb(PIC2_COMMAND,PIC_EOI);
    
        IO::outb(PIC1_COMMAND,PIC_EOI);
    }

    void enable_irq(u8 irq) {

        u16 port;
        if(irq < 8) {
            port = PIC1_DATA;
        } else {
            port = PIC2_DATA;
            irq -= 8;
        }
        u8 orig_mask = IO::inb(port);
        IO::outb(port, orig_mask & ~(1 << irq));  


    }
    void disable_irq(u8 irq) {
        u16 port;
        if(irq < 8) {
            port = PIC1_DATA;
        } else {
            port = PIC2_DATA;
            irq -= 8;
        }
        u8 orig_mask = IO::inb(port);
        IO::outb(port, orig_mask | (1 << irq));  
    }
}