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
    (void)code;
    (void)pid;
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

           WindowServerIPC::CreateWindowResponse response = {w.m_id, w.m_buff_guid};
           std::send_message(pid, (const char*)&response, sizeof(response));
           break;
       } 
       default:
        ASSERT_NOT_REACHED();
    }
}
