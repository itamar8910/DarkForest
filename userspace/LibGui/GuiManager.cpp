#include "GuiManager.h"
#include "kernel/errs.h"
#include "unistd.h"
#include "asserts.h"
#include "LibWindowServer/IPC.h"
#include "Fonts/SimpleFont.h"

GuiManager::GuiManager()
{
    const int rc = std::get_pid_by_name("WindowServer", m_windowserver_pid);
    ASSERT(rc == E_OK);
    kprintf("WindowServer pid: %d\n", m_windowserver_pid);
}


Window GuiManager::create_window(const u16 width, const u16 height)
{
    WindowServerIPC::CreateWindowRequest request = {width, height};
    bool rc;
    rc = WindowServerIPC::send_create_window_request(m_windowserver_pid, request);
    ASSERT(rc);

    WindowServerIPC::CreateWindowResponse response;
    rc =WindowServerIPC::recv_create_window_response(m_windowserver_pid, response, true);
    ASSERT(rc);

    void* buff_addr = 0;
    u32 buff_size = 0;
    rc = std::open_shared_memory(response.buff_guid, buff_addr, buff_size);
    ASSERT(rc == E_OK);

    return Window(
        response.window_guid,
        response.buff_guid,
        buff_addr,
        buff_size,
        width,
        height
    );
}

static GuiManager* s_the = nullptr;

GuiManager& GuiManager::the()
{
    if(s_the == nullptr)
    {
        SimpleFont::initialize();
        s_the = new GuiManager();
    }
    return *s_the;
}

void GuiManager::draw(Window& window)
{
    window.update();

    WindowServerIPC::DrawWindow draw_window_req = {window.id()};
    const bool rc = WindowServerIPC::send_draw_request(m_windowserver_pid, draw_window_req);
    ASSERT(rc);
}

IOEvent GuiManager::get_io_event()
{
    IOEvent io_event;
    const bool rc = WindowServerIPC::recv_io_event(m_windowserver_pid, io_event, true);
    ASSERT(rc);
    return io_event;
}
