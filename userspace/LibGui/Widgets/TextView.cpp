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

    auto do_newline = [&]() 
    {
        current_col = m_x;
        current_row += SIMPLEFONT_SYMBOL_SIZE + 2;
    };

    for(u32 char_idx = 0; char_idx < m_text.len(); ++char_idx)
    {
        if(current_row >= m_height)
        {
            kprintf("\nWARNING: Text Rendering clipped\n");
            break;
        }
        if(m_text[char_idx] == '\n')
        {
            do_newline();
            continue;
        }
        SimpleFont::the().draw(m_text[char_idx], frame_buffer, current_col, current_row, window_width);
        current_col += SIMPLEFONT_SYMBOL_SIZE + 2;
        if(current_col >= m_width)
        {
            do_newline();
            continue;
        }
    }
}
