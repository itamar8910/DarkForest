
#pragma once

#include "types.h"
#include "PS2MouseCommon.h"

constexpr u32 MOUSE_EVENTS_BUFFER_LEN = 20;

class PS2Mouse {
public:
    static PS2Mouse& the();

    static void initialize();

    RawMouseEvent consume();
    bool can_consume();

    void insert_mouse_event(RawMouseEvent event);

private:

    PS2Mouse() = default;

    RawMouseEvent m_events_buffer[MOUSE_EVENTS_BUFFER_LEN];
    u8 m_events_buffer_idx {0};
    u8 m_events_pending {0};

};
