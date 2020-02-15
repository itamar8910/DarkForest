#include "Window.h"
#include "LibWindowServer/IPC.h"
#include "unistd.h"
#include "asserts.h"
#include "kernel/errs.h"

Window::Window(
    u32 window_id,
    u32 buff_guid,
    void* buff_addr,
    u32 buff_size,
    u32 width,
    u32 height) : 
    m_window_id(window_id),
    m_buff_guid(buff_guid),
    m_buff_addr(buff_addr),
    m_buff_size(buff_size),
    m_width(width),
    m_height(height),
    m_background_color(0xffffffff)
    {
        clear();
    }

void Window::clear()
{
    u32* frame_buffer = (u32*) m_buff_addr;
    for(size_t row = 0; row < height(); ++row)
    {
        for(size_t col = 0; col < width(); ++col)
        {
            u32* pixel = frame_buffer + (row*width() + col);
            *pixel = m_background_color;
        }
    }
}

void Window::update()
{
    clear();
}
