
#include "VgaTTY.h"

VgaTTY::VgaTTY() : 
        m_row(0),
        m_column(0),
        m_color(VgaText::compose_color(VgaText::VgaColor::VGA_COLOR_LIGHT_GREY,
            VgaText::VgaColor::VGA_COLOR_BLACK))
    {
        VgaText::clear(m_color);
    }

void VgaTTY::set_color(uint8_t color) {
    m_color = color;
}

void VgaTTY::putchar(char c) 
{
    VgaText::putchar(c, m_color, m_column, m_row);
    if (++m_column == VgaText::VGA_WIDTH) {
        m_column = 0;
        if (++m_row == VgaText::VGA_HEIGHT)
            m_row = 0;
    }
}

void VgaTTY::write(const char* data, size_t size) 
{
    for (size_t i = 0; i < size; i++)
        putchar(data[i]);
}

void VgaTTY::write(const char* str) 
{
    write(str, strlen(str));
}