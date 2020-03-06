#include "IOEvent.h"
#include "asserts.h"

IOEvent::IOEvent(const KeyEvent& event) :
    type(Type::KeyEvent),
    key_event(event)
{
}

IOEvent::IOEvent(const MouseEvent& event) :
    type(Type::MouseEvent),
    mouse_event(event)
{
}

IOEvent::IOEvent() :
    type(Type::Uninitialized),
    key_event()
{
}

KeyEvent IOEvent::as_key_event()
{
    ASSERT(type==Type::KeyEvent);
    return key_event;
}

MouseEvent IOEvent::as_mouse_event()
{
    ASSERT(type==Type::MouseEvent);
    return mouse_event;
}
