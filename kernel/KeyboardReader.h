#pragma once

#include "TaskBlocker.h"
#include "drivers/PS2Keyboard.h"

class KeyboardReadBlocker: public TaskBlocker {
public:
    KeyboardReadBlocker(): TaskBlocker() {}
    virtual bool can_unblock() override;
    virtual ~KeyboardReadBlocker() {}
};

KeyEvent keyboard_read();

