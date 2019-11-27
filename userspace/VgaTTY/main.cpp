#include "unistd.h"
#include "stdio.h"
#include "asserts.h"
#include "mman.h"
#include "string.h"
#include "malloc.h"

#include "ioctl_common.h"
#include "VgaTextCommon.h"
#include "PS2KeyboardCommon.h"



class VgaTTY {
public:
    static const size_t VGA_WIDTH = 80;
    static const size_t VGA_HEIGHT = 25;
    static VgaTTY& the();
    void set_color(uint8_t color);
    void putchar(char c);
    void putchar(char c, u8 color, u8 x, u8 y);
    void write(const char* str);
    void clear(u8 color);
    u16 get_entry(u8 x, u8 y);

private:
    VgaTTY();
    void newline();
    void scrolldown();
    void write(const char* data, size_t size);

    size_t m_row;
    size_t m_column;
    uint8_t m_color;
    int m_vga_text_device_fd;
};

VgaTTY::VgaTTY() : 
        m_row(0),
        m_column(0),
        m_color(VgaTextCommon::compose_color(
                VgaTextCommon::VgaColor::VGA_COLOR_LIGHT_GREY,
                VgaTextCommon::VgaColor::VGA_COLOR_BLACK)
                ),
        m_vga_text_device_fd(-1)
    {

        m_vga_text_device_fd = open("/dev/vgatext");
        ASSERT(m_vga_text_device_fd >= 0, "failed to open vgatext device");
        printf("vgatext fd: %d\n", m_vga_text_device_fd);
        clear(m_color);
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
    IOCTL::VgaText::Data ioctl_data {
            y,
            x,
            0,
        };
    ioctl(m_vga_text_device_fd, static_cast<size_t>(IOCTL::VgaText::Code::GET_ENTRY), &ioctl_data);
    return ioctl_data.value;
}
void VgaTTY::putchar(char c, u8 color, u8 x, u8 y) {
    IOCTL::VgaText::Data ioctl_data {
            static_cast<u8>(y),
            static_cast<u8>(x),
            VgaTextCommon::compose_entry(c, color)
        };
    ioctl(m_vga_text_device_fd, static_cast<size_t>(IOCTL::VgaText::Code::PUT_CHAR), &ioctl_data);
}

void VgaTTY::putchar(char c) 
{
    if(m_row == VgaTTY::VGA_HEIGHT) {
        scrolldown();
        m_row = VgaTTY::VGA_HEIGHT - 1;
    }
    if(c == '\n') { // if newline: col = 0, increment row
        newline();
        return;
    }
    // else, regular char
    putchar(c, m_color, m_column, m_row);
    if (++m_column == VgaTTY::VGA_WIDTH) {
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


int main() {
    printf("hello from: %s!!\n", "VgaTTY!");
    int fd = open("/initrd/hello.txt");
    printf("fd: %d\n", fd);
    int size = file_size(fd);
    printf("file size: %d\n", size);
    char* buff = new char[size+1];
    read(fd, buff, size);
    buff[size] = 0;
    VgaTTY::the().write(buff);

    int keyboard_fd = open("/dev/keyboard");
    ASSERT(keyboard_fd != 0, "err opening keyboard dev");
    KeyEvent key_event;
    while(1) {
        read(keyboard_fd, (char*) &key_event, 1);
		if(!key_event.released && key_event.to_ascii() != 0) {
			VgaTTY::the().putchar(key_event.to_ascii());
		}
    }
    
    return 0;
}