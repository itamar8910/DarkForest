#include "PS2Keyboard.h"
#include "PIC.h"
#include "cpu.h"
#include "logging.h"
#include "IO.h"
#include "asserts.h"
#include "bits.h"

#define PS2_IRQ1 1

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64
#define PS2_COMMAND_PORT 0x64

#define STATUS_OUTPUT (1<<0)
#define STATUS_INPUT (1<<1)

#define PS2_CMD_ECHO 0xEE

#define SCAN_CODE_MULTIBYTE 0xE0

// #define DBG_PS2Keyboard

#define BIT7 128
enum NonAsciiKeys {
    ESC = BIT7 ,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    BackSpace,
    L_CTRL,
    L_SHIFT,
    R_SHIFT,
    L_ALT,
    CapsLock,
    NumLock,
    ScrollLock,
};

const u8 key_map_set1[128] = {
    0, ESC, 
    '1', '2', '3', '4' ,'5' ,'6', '7', '8', '9', '0', // 11 
    '-', '=', BackSpace, '\t', //15
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', // 0x1b
    '\n', L_CTRL, 
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k' , 'l', ';', // 0x27
    '\'', '`', L_SHIFT, '\\', // 0x2b
    'z', 'x' ,'c', 'v', 'b', 'n', 'm', ',', '.', '/', // 0x35
    R_SHIFT, 0, L_ALT, ' ', CapsLock, // 0x3a
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, // 0x44
    NumLock, ScrollLock
};

ISR_HANDLER(ps2_keyboard)
void isr_ps2_keyboard_handler(RegisterDump& regs) {
    (void)regs;
    u8 res = IO::inb(PS2_DATA_PORT);
    #ifdef DBG_PS2Keyboard
    kprintf("keyboard byte: 0x%x\n", res);
    #endif
    PS2Keyboard::the().on_scan_byte(res);
    PIC::eoi(PS2_IRQ1);
}

// TODO: split this to a generic PS2 handler
// PS2Keyboard should use the PS2 handler instead if writing to raw IO ports


u8 PS2Keyboard::get_status() {
    return IO::inb(PS2_STATUS_PORT);
}

void PS2Keyboard::poll_for_write() {
    for(size_t i = 0; i < 10; i++) {
        if(!(get_status() & (STATUS_INPUT))) {
            return;
        }
    }
    ASSERT_NOT_REACHED();
}
void PS2Keyboard::poll_for_read() {
    for(size_t i = 0; i < 10; i++) {
        if((get_status() & (STATUS_OUTPUT))) {
            return;
        }
    }
    ASSERT_NOT_REACHED();
}

static PS2Keyboard* s_the = nullptr;

void PS2Keyboard::clear_input() {

    if((get_status() & (STATUS_OUTPUT))) {
        IO::inb(PS2_DATA_PORT);
    }
}


void PS2Keyboard::initialize() {

    u8 val = get_status();
    kprintf("PS2 status: %d\n", val);
    clear_input();
    poll_for_write();
    IO::outb(PS2_DATA_PORT, PS2_CMD_ECHO);
    poll_for_read();
    u8 res = IO::inb(PS2_DATA_PORT);
    kprintf("PS2 echo res: 0x%x\n", res);
    ASSERT(res == PS2_CMD_ECHO); // According to specs could also be "RESEND"
    
    s_the = new PS2Keyboard();

    register_interrupt_handler(IRQ_ISRS_BASE + PS2_IRQ1, isr_ps2_keyboard_entry);

    PIC::enable_irq(PS2_IRQ1);

}


PS2Keyboard& PS2Keyboard::the() {
    ASSERT(s_the != nullptr);
    return *s_the;

}

PS2Keyboard::PS2Keyboard()
    : m_lock("PS2Keyboard")
 {}


void PS2Keyboard::on_scan_byte(u8 val) {
    add_keycode_byte(val);
    switch(m_current_keycode_byte_idx) {
        case 1:
            {
            if(val == SCAN_CODE_MULTIBYTE) {
                return;
            }
            // create key_code
            bool released = get_bit(val, 7);
            val &= ~(1<<7); // clear 'pressed/realeased bit'
            KeyCode key_code = KeyCode::from_single(val);
            handle_whole_keycode(key_code, released);
            // update modifiers
            if(key_code.data == NonAsciiKeys::L_SHIFT 
                || key_code.data == NonAsciiKeys::R_SHIFT) {
                m_current_modifiers.shift = !released;
            }
            if(key_code.data == NonAsciiKeys::CapsLock 
                && released) {
                m_current_modifiers.caps_lock = ! m_current_modifiers.caps_lock;
            }
            break;
            }
        case 2:
            m_current_keycode_byte_idx = 0;
            break;
        default:
            ASSERT_NOT_REACHED();
    };
}

void PS2Keyboard::add_keycode_byte(u8 val) {
    ASSERT(m_current_keycode_byte_idx < MAX_NUM_KEYCODE_BYTES);
    m_current_keycode_bytes[m_current_keycode_byte_idx++] = val;
}

void PS2Keyboard::handle_whole_keycode(KeyCode key_code, bool released) {
    KeyEvent key_state = KeyEvent(key_code, released, m_current_modifiers);
    #ifdef DBG_PS2Keyboard
    char ascii = key_state.to_ascii();
    if(!released) {
        if(ascii) {
            kprintf("key: %c\n", ascii);
        } else{
            kprint("<NO_ASCII>\n");
        }
    }
    #endif
    insert_key_state(key_state);
    m_current_keycode_byte_idx = 0;

}


KeyCode KeyCode::from_single(u8 val) {
    return key_map_set1[val];
}



void PS2Keyboard::insert_key_state(KeyEvent key_state) {
    LOCKER(m_lock);
    m_events_pending += 1;
    m_events_buffer[m_events_buffer_tail++] = key_state;
    m_events_buffer_tail %= KEYCODES_BUFFER_LEN;
}

KeyEvent PS2Keyboard::consume() {
    LOCKER(m_lock);
    ASSERT(m_events_pending>0);
    m_events_pending -= 1;
    auto event = m_events_buffer[m_events_buffer_head];
    m_events_buffer_head = (m_events_buffer_head+1) % KEYCODES_BUFFER_LEN;
    return event;
}

bool PS2Keyboard::can_consume() {
    LOCKER(m_lock);
    return m_events_pending != 0;
}
