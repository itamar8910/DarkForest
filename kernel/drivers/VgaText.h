#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


namespace VgaText {
    /* Hardware text mode color constants. */

    const size_t VGA_WIDTH = 80;
    const size_t VGA_HEIGHT = 25;
    uint16_t* const VGA_TEXT_BUFF = (uint16_t*) 0xB8000;
    
    void clear(uint8_t color);
    
    void putchar(char c, uint8_t color, size_t x, size_t y);

    uint16_t get_entry(size_t x, size_t y);
    void update_cursor(int x, int y);

}
