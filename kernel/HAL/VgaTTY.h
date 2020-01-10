
#pragma once

#include "types.h"
#include "device.h"

class VgaTTY {
public:
    static const size_t VGA_WIDTH = 80;
    static const size_t VGA_HEIGHT = 25;
    static VgaTTY& the();
    void set_color(uint8_t color);
    void putchar(char c);
    void putchar(char c, u8 color, u8 x, u8 y);
    void write(const char* str);
    void write(const char* data, size_t size);
    void clear(u8 color);
    u16 get_entry(u8 x, u8 y);
    void update_cursor(u8 x, u8 y);

private:
    VgaTTY();
    void newline();
    void scrolldown();

    size_t m_row;
    size_t m_column;
    uint8_t m_color;
};

class VgaTTYDevice : public Device {
public:
    VgaTTYDevice(const Path& path)
        : Device(path) {}

    virtual int read(size_t count, void* buf) override;
    virtual int write(char* data, size_t count) override;
    virtual int ioctl(u32 request, void* buf) override;
};