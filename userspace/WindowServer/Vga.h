#pragma once
#include "types.h"

class VGA final
{
public:
    VGA(); // open /dev/vga, ioctl to get dimensions, mmap framebuffer
    ~VGA();

public:
    u32* frame_buffer() {return m_frame_buffer;}
    u32 width() {return m_width;};
    u32 height() {return m_height;};
    u32 pitch() {return m_pitch;};

    void clear();
    void draw(u32* window_buffer);

private:
    int m_vga_fd {0};
    u32 m_width;
    u32 m_height;
    u32 m_pitch;
    u32* m_frame_buffer {nullptr};
};