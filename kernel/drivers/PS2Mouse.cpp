#include "PS2Mouse.h"
#include "IO.h"
#include "asserts.h"
#include "logging.h"
#include "cpu.h"
#include "PIC.h"


#define PS2_IRQ2 12
#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64
#define PREPARE_FOR_COMMAND 0xd4
#define ENABLE_AUX_PORT 0xA8
#define GET_STATUS 0x20
#define SET_STATUS 0x60
#define BIT_ENABLE_IRQ12 2
#define DISABLE_PACKET_STREAMING 0xF5
#define PS2_ACK 0xFA
#define PS2_MOUSE_SET_DEFAULTS 0xF6
#define ENABLE_PACKET_STREAMING 0xF4
#define STATUS_OUTPUT (1<<0)
#define STATUS_INPUT (1<<1)

static u8 get_status() {
    return IO::inb(PS2_STATUS_PORT);
}

static void poll_for_write() {
    for(size_t i = 0; i < 10; i++) {
        if(!(get_status() & (STATUS_INPUT))) {
            return;
        }
    }
    ASSERT_NOT_REACHED();
}

static void poll_for_read() {
    for(size_t i = 0; i < 10; i++) {
        if((get_status() & (STATUS_OUTPUT))) {
            return;
        }
    }
    ASSERT_NOT_REACHED();
}

static void clear_input() {

    if((get_status() & (STATUS_OUTPUT))) {
        IO::inb(PS2_DATA_PORT);
    }
}

ISR_HANDLER(ps2_mouse)
void isr_ps2_mouse_handler(RegisterDump& regs) {
    (void)regs;
    u8 res = IO::inb(PS2_DATA_PORT);
    kprintf("mouse byte: 0x%x\n", res);
    PIC::eoi(PS2_IRQ2);
}

void PS2Mouse::initialize()
{

    clear_input();

    IO::outb(PS2_STATUS_PORT, PREPARE_FOR_COMMAND);
    poll_for_write();
    IO::outb(PS2_STATUS_PORT, ENABLE_AUX_PORT);

    IO::outb(PS2_STATUS_PORT, PREPARE_FOR_COMMAND);
    poll_for_write();
    IO::outb(PS2_STATUS_PORT, GET_STATUS);
    poll_for_read();
    u8 status = IO::inb(PS2_DATA_PORT);
    kprintf("mouse status: 0x%x\n", status);

    status |= BIT_ENABLE_IRQ12;
    // status &= ~(0x20); // disable mouse clock

    IO::outb(PS2_STATUS_PORT, PREPARE_FOR_COMMAND);
    poll_for_write();
    IO::outb(PS2_STATUS_PORT, SET_STATUS);
    poll_for_write();
    IO::outb(PS2_DATA_PORT, status);

    poll_for_write();
    IO::outb(PS2_STATUS_PORT, PREPARE_FOR_COMMAND);
    poll_for_write();
    IO::outb(PS2_DATA_PORT, DISABLE_PACKET_STREAMING);
    poll_for_read();
    ASSERT(IO::inb(PS2_DATA_PORT) == PS2_ACK);

    IO::outb(PS2_STATUS_PORT, PREPARE_FOR_COMMAND);
    poll_for_write();
    IO::outb(PS2_DATA_PORT, PS2_MOUSE_SET_DEFAULTS);
    poll_for_read();
    ASSERT(IO::inb(PS2_DATA_PORT) == PS2_ACK);

    IO::outb(PS2_STATUS_PORT, PREPARE_FOR_COMMAND);
    poll_for_write();
    IO::outb(PS2_DATA_PORT, ENABLE_PACKET_STREAMING);
    poll_for_read();
    ASSERT(IO::inb(PS2_DATA_PORT) == PS2_ACK);

    register_interrupt_handler(IRQ_ISRS_BASE + PS2_IRQ2, isr_ps2_mouse_entry);
    PIC::enable_irq(PS2_IRQ2);
}