#include "unistd.h"
#include "stdio.h"
#include "asserts.h"
#include "mman.h"
#include "string.h"
#include "malloc.h"
#include "types/vector.h"
#include "types/String.h"
#include "kernel/errs.h"
#include "asserts.h"
#include "LibWindowServer/IPC.h"

struct WindowContext
{
    u32 window_id;
    u32 buff_guid;
    void* buff_addr;
    u32 buff_size;
    u32 width;
    u32 height;
};

WindowContext register_window(const u32 windowserver_pid)
{
    u32 create_widnow_code = WindowServerIPC::Code::CreateWindowRequest;
    std::send_message(windowserver_pid, (const char*)&create_widnow_code, sizeof(create_widnow_code));

    const u32 width = 100;
    const u32 height = 200;
    const WindowServerIPC::CreateWindowRequest request = {width, height};
    int rc = std::send_message(windowserver_pid, (const char*)&request, sizeof(request));
    ASSERT(rc == E_OK);

    u32 response_code = 0;
    u32 tmp_pid;
    ASSERT(std::get_message((char*)&response_code, sizeof(response_code), tmp_pid) == sizeof(response_code));
    ASSERT(response_code == WindowServerIPC::Code::CreateWindowResponse);


    WindowServerIPC::CreateWindowResponse response;
    u32 len = std::get_message((char*)&response, sizeof(response), tmp_pid);
    ASSERT(len == sizeof(response));
    ASSERT(tmp_pid == windowserver_pid);

    void* buff_addr = 0;
    u32 buff_size = 0;
    rc = std::open_shared_memory(response.buff_guid, buff_addr, buff_size);
    ASSERT(rc == E_OK);

    return {
        response.window_guid,
        response.buff_guid,
        buff_addr,
        buff_size,
        width,
        height
    };
}

int main() {
    printf("gui!\n");
    std::sleep_ms(1000);
    u32 window_server_pid = 0;
    const int rc = std::get_pid_by_name("WindowServer", window_server_pid);
    ASSERT(rc == E_OK);
    kprintf("WindowServer pid: %d\n", window_server_pid);


    WindowContext window = register_window(window_server_pid); 

    u32* frame_buffer = (u32*)window.buff_addr;
    for(size_t i = 0;;++i)
    {
        memset(window.buff_addr, 0, window.buff_size);
        for(size_t row = 0; row < window.height; ++row)
        {
            for(size_t col = 0; col < window.width; ++col)
            {
                u32* pixel = frame_buffer + (row*window.width + col);
                *pixel = (0xdeadbeef*i);
            }
        }


        u32 draw_window_code = WindowServerIPC::Code::DrawWindow;
        std::send_message(window_server_pid, (const char*)&draw_window_code, sizeof(draw_window_code));

        WindowServerIPC::DrawWindow draw_window_req = {window.window_id};
        const int msg_rc = std::send_message(window_server_pid, (const char*)&draw_window_req, sizeof(draw_window_req));
        ASSERT(msg_rc == E_OK);
        std::sleep_ms(500);
    }

    return 0;
}
