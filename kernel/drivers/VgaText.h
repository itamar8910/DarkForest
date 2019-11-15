#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


namespace VgaText {
    /* Hardware text mode color constants. */
    enum VgaColor {
        VGA_COLOR_BLACK = 0,
        VGA_COLOR_BLUE = 1,
        VGA_COLOR_GREEN = 2,
        VGA_COLOR_CYAN = 3,
        VGA_COLOR_RED = 4,
        VGA_COLOR_MAGENTA = 5,
        VGA_COLOR_BROWN = 6,
        VGA_COLOR_LIGHT_GREY = 7,
        VGA_COLOR_DARK_GREY = 8,
        VGA_COLOR_LIGHT_BLUE = 9,
        VGA_COLOR_LIGHT_GREEN = 10,
        VGA_COLOR_LIGHT_CYAN = 11,
        VGA_COLOR_LIGHT_RED = 12,
        VGA_COLOR_LIGHT_MAGENTA = 13,
        VGA_COLOR_LIGHT_BROWN = 14,
        VGA_COLOR_WHITE = 15,
    };

    const size_t VGA_WIDTH = 80;
    const size_t VGA_HEIGHT = 25;
    uint16_t* const VGA_TEXT_BUFF = (uint16_t*) 0xB8000;
    
    // color is composed of blink, bg color, fg color
    uint8_t compose_color(VgaColor fg, VgaColor bg);
    
    // A VGA text entry is composed of color, character
    uint16_t compose_entry(unsigned char uc, uint8_t color);
    void decompose_entry(uint16_t entry, unsigned char& uc, uint8_t& color);

    void clear(uint8_t color);
    
    void putchar(char c, uint8_t color, size_t x, size_t y);

    uint16_t get_entry(size_t x, size_t y);

}
