
// based on: 
// - https://wiki.osdev.org/8259_PIC#What_does_the_8259_PIC_do.3F
// - serenity OS

#include "types.h"



// ISRs 0x50-0x5f are handlers for IRQ0-15
#define IRQ_ISRS_BASE 0x50

namespace PIC {
    void initialize();
    void eoi(u8 irq); // signal end of interrupt
    void enable_irq(u8 irq);
    void disable_irq(u8 irq);
}