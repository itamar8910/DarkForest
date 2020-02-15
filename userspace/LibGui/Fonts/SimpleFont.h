#pragma once
#include "types.h"
#include "string.h"


constexpr u8 SIMPLEFONT_SYMBOL_SIZE = 8;

class SimpleFont
{

public:
    static void initialize();
    static SimpleFont& the();

    void draw(const char c, u32* frame_buffer, const u16 x, const u16 y, const u16 framebuffer_width);

private:
    SimpleFont() = default;

private:


    struct Symbol
    {
        // bool pixels[SIMPLEFONT_SYMBOL_SIZE][SIMPLEFONT_SYMBOL_SIZE];
        char pixels[SIMPLEFONT_SYMBOL_SIZE*SIMPLEFONT_SYMBOL_SIZE+1];
    };

    Symbol m_symbols[128] = {};

};