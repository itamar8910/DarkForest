#pragma once

#include "types.h"
#include "PS2KeyboardCommon.h"
#include "PS2MouseCommon.h"

struct MouseEvent
{
    u16 x;
    u16 y;
    bool left_click;
    bool right_click;
};

struct IOEvent
{
    enum class Type
    {
        Uninitialized,
        KeyEvent,
        MouseEvent
    };

    IOEvent(const KeyEvent&);
    IOEvent(const MouseEvent&);
    IOEvent();

    KeyEvent as_key_event();
    MouseEvent as_mouse_event();

    Type type;
    union 
    {
        KeyEvent key_event;
        MouseEvent mouse_event;
    };
    
};
