#include "libc/types.h"

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
