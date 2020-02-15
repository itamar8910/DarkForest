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
    u32 create_widnow_code = WindowServerIPC::Code::CreateWindowRequest;
    std::send_message(m_windowserver_pid, (const char*)&create_widnow_code, sizeof(create_widnow_code));

    const WindowServerIPC::CreateWindowRequest request = {width, height};
    int rc = std::send_message(m_windowserver_pid, (const char*)&request, sizeof(request));
    ASSERT(rc == E_OK);

    u32 response_code = 0;
    u32 tmp_pid;
    ASSERT(std::get_message((char*)&response_code, sizeof(response_code), tmp_pid) == sizeof(response_code));
    ASSERT(response_code == WindowServerIPC::Code::CreateWindowResponse);


    WindowServerIPC::CreateWindowResponse response;
    u32 len = std::get_message((char*)&response, sizeof(response), tmp_pid);
    ASSERT(len == sizeof(response));
    ASSERT(tmp_pid == m_windowserver_pid);

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