#pragma once
#include "types.h"

class Widget
{
public:
    Widget(u16 x, u16 y, u16 width, u16 height);
    virtual ~Widget() = default;

    virtual void draw(u32* frame_buffer, const u32 window_width, const u32 window_height) const = 0;

protected:
    u16 m_x;
    u16 m_y;
    u16 m_width;
    u16 m_height;
};