#pragma once

#include "types.h"

class VgaTTY {
public:
    static const size_t VGA_WIDTH = 80;
    static const size_t VGA_HEIGHT = 25;
    static VgaTTY& the();
    void set_color(uint8_t color);
    void putchar(char c);
    void putchar(char c, u8 color, u8 x, u8 y);
    void write(const char* str);
    void clear(u8 color);
    u16 get_entry(u8 x, u8 y);
    void update_cursor(u8 x, u8 y);

private:
    VgaTTY();
    void newline();
    void scrolldown();
    void write(const char* data, size_t size);

    size_t m_row;
    size_t m_column;
    uint8_t m_color;
    int m_vga_text_device_fd;
};