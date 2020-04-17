#pragma once
#include "types.h"
#include "cstring.h"

constexpr u8 SIMPLEFONT_SYMBOL_SIZE = 8;

class SimpleFont
{

public:
    static void initialize();
    static SimpleFont& the();

    void draw(const char c, u32* frame_buffer, const u16 x, const u16 y, const u16 framebuffer_width);

private:
    SimpleFont() = default;

};
