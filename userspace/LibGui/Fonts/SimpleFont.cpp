#include "SimpleFont.h"
#include "asserts.h"
#include "string.h"
#include "stdio.h"

static SimpleFont* s_the = nullptr;

constexpr char font_A[SIMPLEFONT_SYMBOL_SIZE*SIMPLEFONT_SYMBOL_SIZE+1] = \
    "###**###"
    "##*##*##"
    "#*####*#"
    "********"
    "*######*"
    "*######*"
    "*######*"
    "*######*";

void SimpleFont::initialize()
{
    ASSERT(s_the == nullptr);
    s_the = new SimpleFont();
    strcpy(s_the->m_symbols['A'].pixels, font_A);
}

SimpleFont& SimpleFont::the()
{
    ASSERT(s_the != nullptr);
    return *s_the;
}

void SimpleFont::draw(const char c, u32* frame_buffer, const u16 x, const u16 y, const u16 framebuffer_width)
{
    const char* font = m_symbols[(int)c].pixels;
    if(font[0] == 0)
    {
        kprintf("Unsupported character: %c (%d)\n", c, (int)c);
        ASSERT_NOT_REACHED();
    }

    for(u16 row = y; row < y + SIMPLEFONT_SYMBOL_SIZE; ++row)
    {
        for(u16 col = x; col < x + SIMPLEFONT_SYMBOL_SIZE; ++col)
        {
            if(font[(row-y)*SIMPLEFONT_SYMBOL_SIZE + (col-x)] != '*')
            {
                continue;
            }
            u32* dst_pixel = frame_buffer + (row*framebuffer_width + col);
            *dst_pixel = 0x00000000; // TODO: dynamic color
        }
    }
}