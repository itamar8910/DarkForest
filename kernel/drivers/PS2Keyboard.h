#pragma once

#include "types.h"

// the longer getcode has 6 bytes
#define MAX_NUM_KEYCODE_BYTES 6
#define KEYCODES_BUFFER_LEN 20


struct KeyCode {
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

struct KeyEvent {
    KeyCode keycode;
    bool released; // false for pressed, true for released
    ModifiersState modifiers;
    KeyEvent(): keycode(0), released(false), modifiers() {}
    KeyEvent(KeyCode keycode, bool released, ModifiersState modifiers)
        : keycode(keycode), released(released), modifiers(modifiers)
        {}
    char to_ascii();
};


class PS2Keyboard {
public:
    static void initialize();
    static PS2Keyboard& the();
    void on_scan_byte(u8 val);

    KeyEvent consume();
    bool can_consume();

private:
    PS2Keyboard();
    static inline u8 get_status();
    static void poll_for_write();
    static void poll_for_read();
    static void clear_input();
    void insert_key_state(KeyEvent key_state);

    void add_keycode_byte(u8 val);
    void handle_whole_keycode(KeyCode key_code, bool released);
    

    // circuler buffer that stores KeyEvents
    KeyEvent m_events_buffer[KEYCODES_BUFFER_LEN];
    u8 m_events_buffer_idx {0};
    u8 m_events_pending {0};


    // stores bit-wise state of modifiers (e.g shift, capslock)
    ModifiersState m_current_modifiers;

    u8 m_current_keycode_bytes[MAX_NUM_KEYCODE_BYTES];
    u8 m_current_keycode_byte_idx {0};


};
