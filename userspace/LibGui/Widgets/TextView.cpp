#include "TextView.h"
#include "../Fonts/SimpleFont.h"
#include "types/vector.h"

TextView::TextView(u16 x, u16 y, u16 width, u16 height) :
    Widget(x, y, width, height),
    m_text("")
    {}

void TextView::draw(u32* frame_buffer, const u32 window_width, const u32 window_height) const
{
    (void) window_height;

    u16 current_row = m_y;
    u16 current_col = m_x;

    auto do_newline = [&, this]() 
    {
        current_col = m_x;
        current_row += SIMPLEFONT_SYMBOL_SIZE + 2;
    };

    for(u32 char_idx = get_first_visible_char_idx(); char_idx < m_text.len(); ++char_idx)
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

u32 TextView::get_first_visible_char_idx() const
{
    u16 current_col = m_x;

    u32 num_lines = m_text.len() > 0 ? 1 : 0;

    Vector<u32> lines_first_char_idx;
    u32 char_idx = 0;

    auto do_newline = [&, this]() 
    {
        current_col = m_x;
        lines_first_char_idx.append(char_idx+1);
        ++num_lines;
    };

    for(char_idx = 0; char_idx < m_text.len(); ++char_idx)
    {
        if(m_text[char_idx] == '\n')
        {
            do_newline();
            continue;
        }
        current_col += SIMPLEFONT_SYMBOL_SIZE + 2;
        if(current_col >= m_width)
        {
            do_newline();
            continue;
        }
    }

    const u32 max_num_lines = m_height / (SIMPLEFONT_SYMBOL_SIZE + 2);

    if(num_lines <= max_num_lines)
    {
        return 0;
    }

    const u32 first_visible_line = num_lines - max_num_lines + 1;
    return lines_first_char_idx[first_visible_line];

}
