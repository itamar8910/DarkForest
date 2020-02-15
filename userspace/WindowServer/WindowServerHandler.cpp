#include "WindowServerHandler.h"
#include "unistd.h"
#include "LibWindowServer/IPC.h"
#include "stdio.h"

void WindowServerHandler::run()
{
    while(true)
    {
        u32 code = 0;
        u32 pid = 0;
        kprintf("waiting for message..\n");
        const int rc = std::get_message((char*)&code, sizeof(u32), pid);
        ASSERT(rc == sizeof(u32));
        handle_message_code(code, pid);
    }
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
           u32 tmp_pid = 0;
           u32 size = std::get_message((char*)&request, sizeof(request), tmp_pid);
           // TODO: these asserts don't have to hold (concurrency)
           ASSERT(size == sizeof(request));
           ASSERT(tmp_pid == pid);

           const Window w(request);
           u32 code = WindowServerIPC::Code::CreateWindowResponse;
           std::send_message(pid, (const char*)&code, sizeof(code));

           WindowServerIPC::CreateWindowResponse response = {w.id(), w.buff_guid()};
           std::send_message(pid, (const char*)&response, sizeof(response));
            m_windows.append(w);
           break;
       } 
       case WindowServerIPC::Code::DrawWindow:
       {

           kprintf("draw window\n");
           WindowServerIPC::DrawWindow request;
           u32 tmp_pid = 0;
           u32 size = std::get_message((char*)&request, sizeof(request), tmp_pid);
           // TODO: these asserts don't have to hold (concurrency)
           ASSERT(size == sizeof(request));
           Window window = get_window(request.window_guid);
           m_vga.draw((u32*)window.buff_addr(), window.x(), window.y(), window.width(), window.height());
           break;
       }
       default:
        ASSERT_NOT_REACHED();
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