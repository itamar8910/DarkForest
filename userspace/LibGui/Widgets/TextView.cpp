#include "TextView.h"
#include "../Fonts/SimpleFont.h"

TextView::TextView(u16 x, u16 y, u16 width, u16 height) :
    Widget(x, y, width, height),
    m_text("")
    {}

void TextView::draw(u32* frame_buffer, const u32 window_width, const u32 window_height) const
{
    (void) window_height;

    u16 current_row = m_y;
    u16 current_col = m_x;
    for(u32 char_idx = 0; char_idx < m_text.len(); ++char_idx)
    {
        SimpleFont::the().draw(m_text[char_idx], frame_buffer, current_col, current_row, window_width);
        current_col += SIMPLEFONT_SYMBOL_SIZE + 2;
        if(current_col >= m_width)
        {
            current_col = m_x;
            current_row += SIMPLEFONT_SYMBOL_SIZE + 2;
            if(current_row >= m_height)
            {
                kprintf("WARNING: Text Rendering clipped\n");
                break;
            }
        }
    }

    // for(u16 row = m_y; row < m_y + m_height; ++row)
    // {
    //     for(u16 col = m_x; col < m_x + m_width; ++col)
    //     {
    //         u32* dst_pixel = frame_buffer + (row*window_width + col);
    //         *dst_pixel = 0x00FF00FF;
    //         // break;
    //     }
    //     // break;
    // }
}
