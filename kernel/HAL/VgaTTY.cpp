#include "asserts.h"
#include "string.h"
#include "types.h"
#include "ioctl_common.h"
#include "VgaTextCommon.h"
#include "HAL/VgaTTY.h"
#include "drivers/VgaText.h"
#include "errs.h"

VgaTTY::VgaTTY() : 
        m_row(0),
        m_column(0),
        m_color(VgaTextCommon::compose_color(
                VgaTextCommon::VgaColor::VGA_COLOR_LIGHT_GREY,
                VgaTextCommon::VgaColor::VGA_COLOR_BLACK)
                )
    {
        VgaText::clear(m_color);
    }

void VgaTTY::clear(uint8_t color) {
	for (size_t y = 0; y < VgaTTY::VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VgaTTY::VGA_WIDTH; x++) {
			putchar(' ', color, x, y);
		}
	}
}

void VgaTTY::set_color(uint8_t color) {
    m_color = color;
}

void VgaTTY::newline() {
    m_column = 0;
    m_row++;
}

void VgaTTY::scrolldown() { 
       // move all rows up by one
	for (size_t y = 0; y < VgaTTY::VGA_HEIGHT-1; y++) {
		for (size_t x = 0; x < VgaTTY::VGA_WIDTH; x++) {
            uint16_t nextline_entry = VgaTTY::get_entry(x, y+1);
            unsigned char uc;
            uint8_t color;
            VgaTextCommon::decompose_entry(nextline_entry, uc, color);
        
            VgaTTY::putchar((char)uc, color, x, y);
        }
    }
    // clear last line
    for (size_t x = 0; x < VgaTTY::VGA_WIDTH; x++) {
        VgaTTY::putchar(' ', m_color, x, VgaTTY::VGA_HEIGHT-1);
    }
}

u16 VgaTTY::get_entry(u8 x, u8 y) {
    return VgaText::get_entry(x, y);
}


void VgaTTY::putchar(char c, u8 color, u8 x, u8 y) {
    VgaText::putchar(c, color, x, y);
}

void VgaTTY::update_cursor(u8 x, u8 y) {
    VgaText::update_cursor(x, y);
}

void VgaTTY::putchar(char c) 
{
    if(c == '\n') {
        newline();
    } else if(c == '\b') {
        if(m_column > 0) {
            m_column -= 1;
        }
        else if(m_row > 0) {
            m_row -= 1;
            m_column = VgaTTY::VGA_WIDTH-1;
        } else {
            m_column = 0;
        }
        putchar(' ', m_color, m_column, m_row);
    }
    else {
        // else, regular char
        putchar(c, m_color, m_column, m_row);
        if (++m_column == VgaTTY::VGA_WIDTH) {
            newline();
        }
    }
    if(m_row == VgaTTY::VGA_HEIGHT) {
        scrolldown();
        m_row = VgaTTY::VGA_HEIGHT - 1;
        m_column = 0;
    }
    update_cursor(m_column, m_row);
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

static VgaTTY* s_the = nullptr;

static u8 placeholder[sizeof(VgaTTY)];

VgaTTY& VgaTTY::the() {
    if(!s_the) {
        new(placeholder) VgaTTY();
        s_the = (VgaTTY*) placeholder;
    }
    return *s_the;
}

int VgaTTYDevice::read(size_t count, void* buf)
{
    (void)count;
    (void)buf;
    return E_INVALID;
}
int VgaTTYDevice::write(char* data, size_t count) 
{
    VgaTTY::the().write(data, count);
    return count;
}
int VgaTTYDevice::ioctl(u32 request, void* buf) 
{
    (void)request;
    (void)buf;
    return E_INVALID;
}
