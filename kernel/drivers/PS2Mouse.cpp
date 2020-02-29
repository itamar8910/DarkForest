#include "PS2Mouse.h"
#include "IO.h"
#include "asserts.h"
#include "logging.h"
#include "cpu.h"
#include "PIC.h"
#include "types.h"

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


#define DBG_PS2Mouse

struct [[gnu::packed]] FirstPacket
{
    u8 left_button : 1;
    u8 right_button : 1;
    u8 middle_button : 1;
    u8 always1 : 1;
    u8 x_sign : 1;
    u8 y_sign : 1;
    u8 x_overflow : 1;
    u8 y_overflow : 1;
};

static_assert(sizeof(FirstPacket) == 1);


static PS2Mouse* s_the = nullptr;

PS2Mouse& PS2Mouse::the()
{
    if(s_the == nullptr)
    {
        s_the = new PS2Mouse();
    }
    return *s_the;
}

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
    u8 status = IO::inb(PS2_STATUS_PORT);
    if(!(((status & 1) != 0) && ((status & 0x20) != 0)))
    {
        PIC::eoi(PS2_IRQ2);
        return;
    }
    
    const u8 b1 = IO::inb(PS2_DATA_PORT);
    poll_for_read();
    const u8 b2 = IO::inb(PS2_DATA_PORT);
    poll_for_read();
    const u8 b3 = IO::inb(PS2_DATA_PORT);

    const FirstPacket* first = reinterpret_cast<const FirstPacket*>(&b1);

    int32_t x_delta = static_cast<int32_t>((first->x_sign == 0) ? (b2) : (0xffffff00 | b2));
    int32_t y_delta = static_cast<int32_t>((first->y_sign == 0) ? (b3) : (0xffffff00 | b3));

#ifdef DBG_PS2Mouse
    kprintf("x delta: %d\n", x_delta);
    kprintf("y delta: %d\n", y_delta);
#endif

    PS2Mouse::the().insert_mouse_event({x_delta, y_delta});

    PIC::eoi(PS2_IRQ2);
}

void PS2Mouse::insert_mouse_event(MouseEvent event) {
    m_events_pending += 1;
    m_events_buffer[m_events_buffer_idx++] = event;
    m_events_buffer_idx %= MOUSE_EVENTS_BUFFER_LEN;
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

MouseEvent PS2Mouse::consume() {
    ASSERT(m_events_pending>0);
    m_events_pending -= 1;
    m_events_buffer_idx = (m_events_buffer_idx-1) % MOUSE_EVENTS_BUFFER_LEN;
    auto event = m_events_buffer[m_events_buffer_idx];
    return event;
}

bool PS2Mouse::can_consume() {
    return m_events_pending != 0;
}
