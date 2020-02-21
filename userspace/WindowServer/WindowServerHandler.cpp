#include "WindowServerHandler.h"
#include "unistd.h"
#include "LibWindowServer/IPC.h"
#include "stdio.h"
#include "kernel/errs.h"

WindowServerHandler::WindowServerHandler(VGA& vga) :
    m_vga(vga),
    m_keyboard_fd(std::open("/dev/keyboard")),
    m_windows()
{
}

void WindowServerHandler::run()
{
    while(true)
    {
        u32 code = 0;
        u32 pid = 0;
        kprintf("waiting for message..\n");

        PendingInputBlocker::Reason reason = {};
        u32 ready_fd = 0;
        u32 fds[] = {m_keyboard_fd};
        kprintf("keyboard fd:%d\n", m_keyboard_fd);
        const int rc = std::block_until_pending(fds, 1, ready_fd, reason);
        ASSERT(rc == E_OK);

        if(reason == PendingInputBlocker::Reason::PendingMessage)
        {
            const int rc = std::get_message((char*)&code, sizeof(u32), pid);
            ASSERT(rc == sizeof(u32));
            handle_message_code(code, pid);
        }
        else if(reason == PendingInputBlocker::Reason::FdReady)
        {
            ASSERT(ready_fd == m_keyboard_fd);
            handle_pending_keyboard_event();
        }
    }
}

void WindowServerHandler::handle_pending_keyboard_event()
{
    KeyEvent key_event;
    const int read_rc = std::read(m_keyboard_fd, reinterpret_cast<char*>(&key_event), 1);
    ASSERT(read_rc == 1);
    kprintf("key event: %c\n", key_event.to_ascii());
    // TODO: send event to currently focues window
}

void WindowServerHandler::handle_message_code(u32 code, u32 pid)
{
    (void)pid;

    kprintf("handle_message_code: %d\n", code);

    switch(code)
    {
       case WindowServerIPC::Code::CreateWindowRequest:
       {
           WindowServerIPC::CreateWindowRequest request;

            bool rc;
            rc = WindowServerIPC::recv_create_window_request(pid, request, false);
            ASSERT(rc);

            const Window w(request);

            WindowServerIPC::CreateWindowResponse response = {w.id(), w.buff_guid()};
            rc = WindowServerIPC::send_create_window_response(pid, response);
            ASSERT(rc);

            m_windows.append(w);
           break;
       } 

       case WindowServerIPC::Code::DrawWindow:
       {

           kprintf("draw window\n");
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