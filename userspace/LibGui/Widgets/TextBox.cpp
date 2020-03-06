#include "../Fonts/SimpleFont.h"
#include "TextBox.h"

TextBox::TextBox(u16 x, u16 y, u16 width, u16 height) :
    Widget(x, y, width, height),
    m_cursor_position({0,0})
{

    for (u32 x = 0; x < m_width / (SIMPLEFONT_SYMBOL_SIZE + 2); ++x)
    {
        for (u32 y = 0; y < m_height / (SIMPLEFONT_SYMBOL_SIZE + 2); ++y)
        {
            m_screen_text.append(' ');
        }
    }
}

void TextBox::draw(u32* frame_buffer, const u32 window_width, const u32 /*window_height*/) const
{
    for (u32 y = 0; y < m_height / (SIMPLEFONT_SYMBOL_SIZE + 2); ++y)
    {
        for (u32 x = 0; x < m_width / (SIMPLEFONT_SYMBOL_SIZE + 2); ++x)
        {
            SimpleFont::the().draw(m_screen_text[x + y * m_width / (SIMPLEFONT_SYMBOL_SIZE + 2)], frame_buffer, x * (SIMPLEFONT_SYMBOL_SIZE + 2), y * (SIMPLEFONT_SYMBOL_SIZE + 2), window_width);
        }
    }

    // TODO: we need a proper cursor
    SimpleFont::the().draw('*', frame_buffer, m_cursor_position.x * (SIMPLEFONT_SYMBOL_SIZE + 2), m_cursor_position.y * (SIMPLEFONT_SYMBOL_SIZE + 2), window_width);
}

void TextBox::set_cursor_position(Point position)
{
    m_cursor_position = position;
}

Point TextBox::get_cursor_position()
{
    return m_cursor_position;
}

void TextBox::set_character(char c, Point position)
{
    m_screen_text[position.x + position.y * m_width / (SIMPLEFONT_SYMBOL_SIZE + 2)] = c;
}


