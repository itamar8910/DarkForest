#include "TextView.h"

TextView::TextView(u16 x, u16 y, u16 width, u16 height) :
    Widget(x, y, width, height),
    m_text("")
    {}

void TextView::draw(u32* frame_buffer, const u32 window_width, const u32 window_height) const
{
    (void) window_height;
    kprintf("TV: %d,%d,%d,%d\n~~~~~~~~~~~~~~~~~~~~~~~\n", m_x, m_y, m_width, m_height);
    for(u16 row = m_y; row < m_y + m_height; ++row)
    {
        for(u16 col = m_x; col < m_x + m_width; ++col)
        {
            u32* dst_pixel = frame_buffer + (row*window_width + col);
            *dst_pixel = 0x00FF00FF;
            // break;
        }
        // break;
    }
}
