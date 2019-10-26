#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "string.h"

#include "VgaText.h"

class VgaTTY {

public:
    static VgaTTY& the();
    void set_color(uint8_t color);
    void putchar(char c);
    void write(const char* str);

private:
    VgaTTY();
    void newline();
    void scrolldown();
    void write(const char* data, size_t size);

    size_t m_row;
    size_t m_column;
    uint8_t m_color;
};
