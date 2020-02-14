#include "Window.h"
#include "unistd.h"
#include "asserts.h"
#include "kernel/errs.h"

Window::Window(const WindowServerIPC::CreateWindowRequest& request)
{
    const u32 window_guid = std::generate_guid();
    const u32 buff_guid = std::generate_guid();
    const u32 buff_size = request.height * request.width * sizeof(u32);
    void* buff_addr = nullptr;
    const int rc = std::create_shared_memory(
                            buff_guid,
                            buff_size,
                            buff_addr);
    ASSERT(rc == E_OK);

    m_id = window_guid;
    m_buff_guid =  buff_guid;
    m_buff_addr = {nullptr};
    m_x = 50;
    m_y = 100;
    m_width = request.width;
    m_height = request.height;
}