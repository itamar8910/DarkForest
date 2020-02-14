#include "Window.h"
#include "unistd.h"
#include "asserts.h"
#include "kernel/errs.h"
#include "Math.h"
#include "constants.h"

constexpr u32 DEFAULT_X = 100;
constexpr u32 DEFAULT_Y = 100;

Window::Window(const WindowServerIPC::CreateWindowRequest& request)
{
    const u32 window_guid = std::generate_guid();
    const u32 buff_guid = std::generate_guid();
    const u32 buff_size = request.height * request.width * sizeof(u32);
    void* buff_addr = nullptr;
    const int rc = std::create_shared_memory(
                            buff_guid,
                            Math::round_up(buff_size, PAGE_SIZE),
                            buff_addr);
    kprintf("size: %d\n", buff_size);
    kprintf("shared mem rc: %d\n", rc);
    ASSERT(rc == E_OK);

    m_id = window_guid;
    m_buff_guid =  buff_guid;
    m_buff_addr = buff_addr;
    m_buff_size = buff_size;
    m_x = DEFAULT_X;
    m_y = DEFAULT_Y;
    m_width = request.width;
    m_height = request.height;
}