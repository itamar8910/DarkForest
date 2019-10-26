#include "PS2Keyboard.h"
#include "PIC.h"
#include "cpu.h"
#include "logging.h"
#include "IO.h"
#include "Kassert.h"
#include "bits.h"

#define PS2_IRQ1 1
#define PS2_IRQ2 12

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64
#define PS2_COMMAND_PORT 0x64

#define STATUS_OUTPUT (1<<0)
#define STATUS_INPUT (1<<1)

#define PS2_CMD_ECHO 0xEE

#define SCAN_CODE_MULTIBYTE 0xE0

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
    TAB,
    ENTER,
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
    '-', '=', BackSpace, TAB, //15
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', // 0x1b
    ENTER, L_CTRL, 
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
    kprintf("PS2Keyboard ISR\n");
    u8 res = IO::inb(PS2_DATA_PORT);
    kprintf("keyboard byte: 0x%x\n", res);
    PS2Keyboard::the().received_scan_byte(res);
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
    ASSERT_NOT_REACHED("PS2Keyboard: poll for write exceeded");
}
void PS2Keyboard::poll_for_read() {
    for(size_t i = 0; i < 10; i++) {
        if((get_status() & (STATUS_OUTPUT))) {
            return;
        }
    }
    ASSERT_NOT_REACHED("PS2Keyboard: poll for write exceeded");
}

static PS2Keyboard* s_the = nullptr;

void PS2Keyboard::initialize() {

    u8 val = get_status();
    kprintf("PS2 status: %d\n", val);
    poll_for_write();
    IO::outb(PS2_DATA_PORT, PS2_CMD_ECHO);
    poll_for_read();
    u8 res = IO::inb(PS2_DATA_PORT);
    kprintf("PS2 echo res: 0x%x\n", res);
    ASSERT(res == PS2_CMD_ECHO); // According to specs could also be "RESEND"
    
    s_the = new PS2Keyboard();

    register_interrupt_handler(IRQ_ISRS_BASE + PS2_IRQ1, isr_ps2_keyboard_entry);

    PIC::enable_irq(PS2_IRQ1);
    PIC::enable_irq(PS2_IRQ2);

}


PS2Keyboard& PS2Keyboard::the() {
    ASSERT(s_the != nullptr);
    return *s_the;

}

PS2Keyboard::PS2Keyboard() 
    : current_keycode_bytes{0},
      current_keycode_byte_idx{0},
      keycodes_buffer_idx{0} {}


void PS2Keyboard::received_scan_byte(u8 val) {
    current_keycode_bytes[current_keycode_byte_idx++] = val;
    switch(current_keycode_byte_idx) {
        case 1:
            {
            if(val == SCAN_CODE_MULTIBYTE) {
                return;
            }
            bool released = get_bit(val, 7);
            val &= ~(1<<7); // clear 'pressed/realeased bit'
            KeyCode key_code = KeyCode::from_single(val);
            KeyState key_state = KeyState(key_code, released, current_modifiers);
            char ascii = key_state.to_ascii();
            if(!released) {
                if(ascii) {
                    kprintf("key: %c\n", ascii);
                } else{
                    kprint("<NO_ASCII>\n");
                }
            }
            keycodes_buffer[keycodes_buffer_idx++] = key_state;
            keycodes_buffer_idx %= KEYCODES_BUFFER_LEN;
            current_keycode_byte_idx = 0;
            // set modifiers
            if(key_code.data == NonAsciiKeys::L_SHIFT 
                || key_code.data == NonAsciiKeys::R_SHIFT) {
                current_modifiers.shift = !released;
            }
            if(key_code.data == NonAsciiKeys::CapsLock 
                && released) {
                current_modifiers.caps_lock = ! current_modifiers.caps_lock;
            }
            break;
            }
        case 2:
            break;
        default:
            ASSERT_NOT_REACHED("PS2Keyboard: invalid scan code length");
    };
}


KeyCode KeyCode::from_single(u8 val) {
    return key_map_set1[val];
}

const char nums_upper[10] = {')', '!', '@', '#', '$', '%', '^', '&', '*', '('};

static char to_upper(char val) {
    ASSERT(!get_bit(val, 7));
    if(val >= '0' && val <= '9') {
        return nums_upper[(u8)(val-'0')];
    }
    if(val >= 'a' && val <= 'z') {
        return val - 0x20;
    }
    switch(val) {
        case '-':
            return '_';
        case '=':
            return '+';
        case '[':
            return '{';
        case ']':
            return '}';
        case ';':
            return ':';
        case '\'':
            return '"';
        case '\\':
            return '|';
        case ',':
            return '<';
        case '.':
            return '>';
        case '/':
            return '?';
        case '`':
            return '~';
    }
    ASSERT_NOT_REACHED("KeyBoard: to_upper - upper vale not found for char value");
    return 0;
}

char KeyState::to_ascii() {
    // valid ascii has bit 7 clear
    if(get_bit(keycode.data, 7)) {
        return 0;
    }
    if(modifiers.caps_lock | modifiers.shift) {
        return to_upper(keycode.data);
    }
    return static_cast<char>(keycode.data);
}