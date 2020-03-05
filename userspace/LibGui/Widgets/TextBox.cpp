#include "../Fonts/SimpleFont.h"
#include "TextBox.h"

TextBox::TextBox(u16 x, u16 y, u16 width, u16 height) :
    Widget(x, y, width, height),
    m_cursor_position({0,0})
{

    for (u32 x = 0; x < m_width; ++x)
    {
        for (u32 y = 0; y < m_height; ++y)
        {
            m_screen_text.append(' ');
        }
    }
}

void TextBox::draw(u32* frame_buffer, const u32 window_width, const u32 /*window_height*/) const
{
    for (u32 x = 0; x < m_width; ++x)
    {
        for (u32 y = 0; y < m_height; ++y)
        {
            SimpleFont::the().draw(m_screen_text[x + y * m_width], frame_buffer, x, y, window_width);
        }
    }

    // TODO: we need a proper cursor
    SimpleFont::the().draw('*', frame_buffer, m_cursor_position.x, m_cursor_position.y, window_width);
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
    m_screen_text[position.x + position.y * m_width] = c;
}


