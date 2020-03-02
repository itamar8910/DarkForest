#include "WindowServerHandler.h"
#include "unistd.h"
#include "LibWindowServer/IPC.h"
#include "stdio.h"
#include "kernel/errs.h"
#include "PS2MouseCommon.h"
#include "Math.h"

u32 WindowServerHandler::mouse_sprite_buffer[MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE] = {};

WindowServerHandler::WindowServerHandler(VGA& vga) :
    m_vga(vga),
    m_keyboard_fd(std::open("/dev/keyboard")),
    m_mouse_fd(std::open("/dev/mouse")),
    m_windows(),
    m_hidden_by_mouse(MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE)
{
    ASSERT(m_keyboard_fd >=0);
    ASSERT(m_mouse_fd >=0);
    m_mouseX = (m_vga.width()) / 2;
    m_mouseY = (m_vga.height()) / 2;
    m_vga.clear();

    init_mouse_sprite();
    memset(m_hidden_by_mouse.data(), 0, MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE*sizeof(u32));
    m_hidden_by_mouse.set_size(MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE);
}

void WindowServerHandler::run()
{
    while(true)
    {
        u32 code = 0;
        u32 pid = 0;

        PendingInputBlocker::Reason reason = {};
        u32 ready_fd = 0;
        constexpr u32 NUM_FDS = 2;
        u32 fds[] = {(u32)m_keyboard_fd, (u32)m_mouse_fd};
        const int rc = std::block_until_pending(fds, NUM_FDS, ready_fd, reason);
        ASSERT(rc == E_OK);

        if(reason == PendingInputBlocker::Reason::PendingMessage)
        {
            const int rc = std::get_message((char*)&code, sizeof(u32), pid);
            ASSERT(rc == sizeof(u32));
            handle_message_code(code, pid);
        }
        else if(reason == PendingInputBlocker::Reason::FdReady)
        {
            kprintf("kb fd: %d\n", m_keyboard_fd);
            kprintf("ms fd: %d\n", m_mouse_fd);
            kprintf("ready fd: %d\n", ready_fd);
            if(ready_fd == (u32)m_keyboard_fd)
            {
                handle_pending_keyboard_event();
                continue;
            }
            else if(ready_fd == (u32)m_mouse_fd)
            {
                handle_pending_mouse_event();
                continue;
            }
            ASSERT_NOT_REACHED();
        }
    }
}

void WindowServerHandler::handle_pending_keyboard_event()
{
    KeyEvent key_event;
    const int read_rc = std::read(m_keyboard_fd, reinterpret_cast<char*>(&key_event), 1);
    ASSERT(read_rc == 1);

    // TODO: only send event to currently focued window

    for(auto& window : m_windows)
    {
        WindowServerIPC::send_key_event(window.owner_pid(), key_event);
    }
}

void WindowServerHandler::handle_pending_mouse_event()
{
    MouseEvent event;
    const int read_rc = std::read(m_mouse_fd, reinterpret_cast<char*>(&event), 1);
    ASSERT(read_rc == 1);

    int previous_mouseX = m_mouseX;
    int previous_mouseY = m_mouseY;

    m_mouseX = Math::clamp(m_mouseX + event.delta_x, 0, m_vga.width() - MOUSE_SPRITE_SIZE - 1);
    m_mouseY = Math::clamp(m_mouseY - event.delta_y, 0, m_vga.height() - MOUSE_SPRITE_SIZE - 1);

    kprintf("WinodwServer: Mouse: %d,%d\n", m_mouseX, m_mouseY);

    // memset(mouse_sprite_buffer, 0xff, sizeof(mouse_sprite_buffer));

    Vector<u32> tmp_hidden_pixels = m_hidden_by_mouse;

    m_vga.draw(tmp_hidden_pixels.data(), previous_mouseX, previous_mouseY, MOUSE_SPRITE_SIZE, MOUSE_SPRITE_SIZE); 
    m_vga.copy_framebuffer_section(m_hidden_by_mouse.data(), m_mouseX, m_mouseY, MOUSE_SPRITE_SIZE, MOUSE_SPRITE_SIZE);

    m_vga.draw(mouse_sprite_buffer, m_mouseX, m_mouseY, MOUSE_SPRITE_SIZE, MOUSE_SPRITE_SIZE); 
}

void WindowServerHandler::handle_message_code(u32 code, u32 pid)
{

    switch(code)
    {
       case WindowServerIPC::Code::CreateWindowRequest:
       {
           WindowServerIPC::CreateWindowRequest request;

            bool rc;
            rc = WindowServerIPC::recv_create_window_request(pid, request, false);
            ASSERT(rc);

            const Window w(request, pid);

            WindowServerIPC::CreateWindowResponse response = {w.id(), w.buff_guid()};
            rc = WindowServerIPC::send_create_window_response(pid, response);
            ASSERT(rc);

            m_windows.append(w);
           break;
       } 

       case WindowServerIPC::Code::DrawWindow:
       {

           WindowServerIPC::DrawWindow request;
           const bool rc = WindowServerIPC::recv_draw_request(pid, request, false);
           ASSERT(rc);

           Window window = get_window(request.window_guid);
           m_vga.draw((u32*)window.buff_addr(), window.x(), window.y(), window.width(), window.height());
           break;
       }

       default:
        {
           ASSERT_NOT_REACHED();
        }
    }
}

Window WindowServerHandler::get_window(u32 window_id)
{
    for(auto& window : m_windows)
    {
        if(window.id() == window_id)
        {
            return window;
        }
    }
    ASSERT_NOT_REACHED();
}

void WindowServerHandler::init_mouse_sprite()
{
    char sprite[MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE+1] =  \
        "*******"
        "****** "
        "*****  "
        "****   "
        "*** *  "
        "**   * "
        "*     *";
    
    for(uint32_t i = 0; i < MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE; ++i)
    {
        mouse_sprite_buffer[i] = (sprite[i] == '*') ? 0xffffffff : 0;
    }
}