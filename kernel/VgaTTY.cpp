
#include "VgaTTY.h"
#include "FileSystem/VFS.h"
#include "logging.h"
#include "VgaTextCommon.h"
#include "ioctl_common.h"

VgaTTY::VgaTTY() : 
        m_row(0),
        m_column(0),
        m_color(VgaTextCommon::compose_color(
                VgaTextCommon::VgaColor::VGA_COLOR_LIGHT_GREY,
                VgaTextCommon::VgaColor::VGA_COLOR_BLACK)
                ),
        m_vgatext_device(
                *static_cast<VgaTextDevice*>(
                    VFS::the().open("/dev/vgatext")
                )
            )
    {
        ASSERT(&m_vgatext_device != 0, "failed to create VgaTextDevice");
        VgaText::clear(m_color);
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
	for (size_t y = 0; y < VgaText::VGA_HEIGHT-1; y++) {
		for (size_t x = 0; x < VgaText::VGA_WIDTH; x++) {
            uint16_t nextline_entry = VgaText::get_entry(x, y+1);
            unsigned char uc;
            uint8_t color;
            VgaTextCommon::decompose_entry(nextline_entry, uc, color);
            VgaText::putchar((char)uc, color, x, y);
        }
    }
    // clear last line
    for (size_t x = 0; x < VgaText::VGA_WIDTH; x++) {
        VgaText::putchar(' ', m_color, x, VgaText::VGA_HEIGHT-1);
    }
}

void VgaTTY::putchar(char c) 
{
    if(m_row == VgaText::VGA_HEIGHT) {
        scrolldown();
        m_row = VgaText::VGA_HEIGHT - 1;
    }
    if(c == '\n') { // if newline: col = 0, increment row
        newline();
        return;
    }
    // else, regular char
    IOCTL::VgaText::Data ioctl_data {
            static_cast<u8>(m_row),
            static_cast<u8>(m_column),
            VgaTextCommon::compose_entry(c, m_color)
        };
    m_vgatext_device.ioctl(
        static_cast<int>(IOCTL::VgaText::Code::PUT_CHAR), 
        &ioctl_data
    );
    if (++m_column == VgaText::VGA_WIDTH) {
        newline();
    }
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
VgaTTY& VgaTTY::the() {
    if(!s_the) {
        s_the = new VgaTTY();
    }
    return *s_the;
}