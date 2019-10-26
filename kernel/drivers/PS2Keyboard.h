#pragma once

#include "types.h"

// the longer getcode has 6 bytes
#define MAX_NUM_KEYCODE_BYTES 6
#define KEYCODES_BUFFER_LEN 20


struct KeyCode {
    // upper 3 bits = row, lower 5 bits = col
    u8 data;
    KeyCode(u8 data): data(data) {}
    static KeyCode from_single(u8 val);
};

struct ModifiersState {
    bool shift : 1;
    bool caps_lock : 1;
    ModifiersState() {
        shift = false;
        caps_lock = false;
    }
};

struct KeyState {
    KeyCode keycode;
    bool released; // false for pressed, true for released
    ModifiersState modifiers;
    KeyState(): keycode(0), released(false), modifiers() {}
    KeyState(KeyCode keycode, bool released, ModifiersState modifiers)
        : keycode(keycode), released(released), modifiers(modifiers)
        {}
    char to_ascii();
};


class PS2Keyboard {
public:
    static void initialize();
    static PS2Keyboard& the();
    void received_scan_byte(u8 val);
private:
    PS2Keyboard();
    static inline u8 get_status();
    static void poll_for_write();
    static void poll_for_read();

    // stores bit-wise state of modifiers (e.g shift, capslock)
    ModifiersState current_modifiers;

    u8 current_keycode_bytes[MAX_NUM_KEYCODE_BYTES];
    u8 current_keycode_byte_idx;

    KeyState keycodes_buffer[KEYCODES_BUFFER_LEN]; // circuler buffer that stores KeyCodes
    u8 keycodes_buffer_idx;

};
