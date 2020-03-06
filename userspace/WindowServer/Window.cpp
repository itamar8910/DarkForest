#include "Window.h"
#include "unistd.h"
#include "asserts.h"
#include "kernel/errs.h"
#include "Math.h"
#include "constants.h"

constexpr u32 DEFAULT_X = 100;
constexpr u32 DEFAULT_Y = 100;

static u32 x_offset = 0;
static u32 y_offset = 0;

Window::Window(const WindowServerIPC::CreateWindowRequest& request, u32 pid) :
    m_id(std::generate_guid()),
    m_owner_pid(pid),
    m_buff_guid(std::generate_guid())
{
    const u32 buff_size = request.height * request.width * sizeof(u32);
    void* buff_addr = nullptr;
    const int rc = std::create_shared_memory(
                            m_buff_guid,
                            Math::round_up(buff_size, PAGE_SIZE),
                            buff_addr);
    kprintf("size: %d\n", buff_size);
    kprintf("shared mem rc: %d\n", rc);
    ASSERT(rc == E_OK);

    m_buff_addr = buff_addr;
    m_buff_size = buff_size;
    m_x = DEFAULT_X ;
    m_y = DEFAULT_Y;
    m_width = request.width;
    m_height = request.height;


    // TODO this is just for testing purposes
    m_x += x_offset;
    m_y += y_offset;
    x_offset += m_width + 15;
    y_offset += 10;
}

Rectangle Window::rectangle() const
{
    return {m_x, m_y, m_width, m_height};
}
