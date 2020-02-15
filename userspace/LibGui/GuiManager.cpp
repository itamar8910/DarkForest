#include "GuiManager.h"
#include "kernel/errs.h"
#include "unistd.h"
#include "asserts.h"
#include "LibWindowServer/IPC.h"

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
        s_the = new GuiManager();
    }
    return *s_the;
}

void GuiManager::draw(Window& window)
{
    window.update();

    u32 draw_window_code = WindowServerIPC::Code::DrawWindow;
    std::send_message(m_windowserver_pid, (const char*)&draw_window_code, sizeof(draw_window_code));

    WindowServerIPC::DrawWindow draw_window_req = {window.id()};
    const int msg_rc = std::send_message(m_windowserver_pid, (const char*)&draw_window_req, sizeof(draw_window_req));
    ASSERT(msg_rc == E_OK);
}