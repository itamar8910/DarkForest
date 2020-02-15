#include "SimpleFont.h"
#include "asserts.h"
#include "string.h"
#include "stdio.h"
#include "SimpleFontSymbols.h"

static SimpleFont* s_the = nullptr;

void SimpleFont::initialize()
{
    ASSERT(s_the == nullptr);
    s_the = new SimpleFont();
}

SimpleFont& SimpleFont::the()
{
    ASSERT(s_the != nullptr);
    return *s_the;
}

void SimpleFont::draw(const char c, u32* frame_buffer, const u16 x, const u16 y, const u16 framebuffer_width)
{

    for(u16 row = y; row < y + SIMPLEFONT_SYMBOL_SIZE; ++row)
    {
        for(u16 col = x; col < x + SIMPLEFONT_SYMBOL_SIZE; ++col)
        {
            if(!is_on(c, static_cast<u8>(col-x), static_cast<u8>(row-y)))
            {
                continue;
            }
            u32* dst_pixel = frame_buffer + (row*framebuffer_width + col);
            *dst_pixel = 0x00000000; // TODO: dynamic color
        }
    }
}