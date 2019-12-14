
#include "PS2KeyboardCommon.h"
#include "libc/bits.h"

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
    return val;
}

constexpr u8 KEYCODE_BACKSPACE = 139;

char KeyEvent::to_ascii() {
    // valid ascii has bit 7 clear
    if(get_bit(keycode.data, 7)) {
        if(keycode.data == KEYCODE_BACKSPACE) {
            return '\b';
        }
        return 0;
    }
    if(modifiers.caps_lock | modifiers.shift) {
        return to_upper(keycode.data);
    }
    return static_cast<char>(keycode.data);
}