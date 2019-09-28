#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "Cstring.h"

#include "VgaTextDriver.h"

class VgaTTY {
private:
    size_t m_row;
    size_t m_column;
    uint8_t m_color;
public:
    VgaTTY();
    void set_color(uint8_t color);

    void putchar(char c);

    void write(const char* data, size_t size);
    
    void write(const char* str);
};
