#include "PS2Keyboard.h"
#include "PIC.h"
#include "cpu.h"
#include "logging.h"
#include "IO.h"
#include "Kassert.h"

#define PS2_IRQ1 1
#define PS2_IRQ2 12

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64
#define PS2_COMMAND_PORT 0x64

#define STATUS_OUTPUT (1<<0)
#define STATUS_INPUT (1<<1)

#define PS2_CMD_ECHO 0xEE


ISR_HANDLER(ps2_keyboard)
void isr_ps2_keyboard_handler(RegisterDump& regs) {
    (void)regs;
    kprintf("PS2Keyboard ISR\n");
    u8 res = IO::inb(PS2_DATA_PORT);
    kprintf("keyboard data: 0x%x\n", res);
    PIC::eoi(PS2_IRQ1);
}

// TODO: split this to a generic PS2 handler
// PS2Keyboard should use the PS2 handler instead if writing to raw IO ports

namespace PS2Keyboard {

    u8 get_status() {
        return IO::inb(PS2_STATUS_PORT);
    }

    void poll_for_write() {
        for(size_t i = 0; i < 10; i++) {
            if(!(get_status() & (STATUS_INPUT))) {
                return;
            }
        }
        ASSERT_NOT_REACHED("PS2Keyboard: poll for write exceeded");
    }
    void poll_for_read() {
        for(size_t i = 0; i < 10; i++) {
            if((get_status() & (STATUS_OUTPUT))) {
                return;
            }
        }
        ASSERT_NOT_REACHED("PS2Keyboard: poll for write exceeded");
    }

    void initialize() {

        u8 val = get_status();
        kprintf("PS2 status: %d\n", val);
        poll_for_write();
        IO::outb(PS2_DATA_PORT, PS2_CMD_ECHO);
        poll_for_read();
        u8 res = IO::inb(PS2_DATA_PORT);
        kprintf("PS2 echo res: 0x%x\n", res);
        ASSERT(res == PS2_CMD_ECHO); // According to specs could also be "RESEND"
        
        register_interrupt_handler(IRQ_ISRS_BASE + PS2_IRQ1, isr_ps2_keyboard_entry);

        PIC::enable_irq(PS2_IRQ1);
        PIC::enable_irq(PS2_IRQ2);
    }
}