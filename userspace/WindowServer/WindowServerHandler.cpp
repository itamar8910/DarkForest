#include "WindowServerHandler.h"
#include "unistd.h"
#include "LibWindowServer/IPC.h"
#include "stdio.h"
#include "kernel/errs.h"
#include "PS2MouseCommon.h"
#include "Math.h"

WindowServerHandler::WindowServerHandler(VGA& vga) :
    m_vga(vga),
    m_keyboard_fd(std::open("/dev/keyboard")),
    m_mouse_fd(std::open("/dev/mouse")),
    m_windows(),
    m_mouse((m_vga.width()) / 2, (m_vga.height()) / 2)
{
    ASSERT(m_keyboard_fd >=0);
    ASSERT(m_mouse_fd >=0);
    m_vga.clear();
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

    m_mouse.draw(event, m_vga);
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
