#pragma once

#include "PS2KeyboardCommon.h"
#include "PS2MouseCommon.h"

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
