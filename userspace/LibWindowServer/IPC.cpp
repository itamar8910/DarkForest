#include "IPC.h"
#include "df_unistd.h"
#include "kernel/errs.h"

namespace WindowServerIPC
{

bool send_create_window_request(u32 windowserver_pid, const CreateWindowRequest& request)
{
    u32 create_widnow_code = WindowServerIPC::Code::CreateWindowRequest;
    int rc = 0;
    rc = std::send_message(windowserver_pid, (const char*)&create_widnow_code, sizeof(create_widnow_code));
    if(rc != E_OK)
    {
        return false;
    }

    rc = std::send_message(windowserver_pid, (const char*)&request, sizeof(request));
    if(rc != E_OK)
    {
        return false;
    }

    return true;
}


bool recv_create_window_request(u32 gui_pid, CreateWindowRequest& request, bool recv_code)
{
    ASSERT(recv_code == false); //not implemented

    u32 tmp_pid = 0;
    u32 size = std::get_message((char*)&request, sizeof(request), tmp_pid);

    if(size != sizeof(request))
    {
        return false;
    }
    if(tmp_pid != gui_pid)
    {
        return false;
    }
    return true;
}

bool send_create_window_response(u32 gui_pid, const CreateWindowResponse& resp)
{
    int rc;
    u32 code = WindowServerIPC::Code::CreateWindowResponse;
    rc = std::send_message(gui_pid, (const char*)&code, sizeof(code));
    if(rc != E_OK)
    {
        return false;
    }

    rc = std::send_message(gui_pid, (const char*)&resp, sizeof(resp));
    if(rc != E_OK)
    {
        return false;
    }
    return true;
}

bool recv_create_window_response(u32 windowserver_pid, CreateWindowResponse& resp, bool recv_code)
{
    u32 tmp_pid;
    int rc = 0;
    if(recv_code)
    {
        u32 response_code = 0;
        rc = std::get_message((char*)&response_code, sizeof(response_code), tmp_pid);
        if(rc != sizeof(response_code))
        {
            return false;
        }
        if(response_code != WindowServerIPC::Code::CreateWindowResponse)
        {
            return false;
        }
        if(tmp_pid != windowserver_pid)
        {
            return false;
        }

    }


    const u32 len = std::get_message((char*)&resp, sizeof(resp), tmp_pid);
    if(len != sizeof(resp))
    {
        return false;
    }
    if(tmp_pid != windowserver_pid)
    {
        return false;
    }
    return true;
}

bool send_draw_request(u32 windowserver_pid, const DrawWindow& request)
{
    u32 draw_window_code = WindowServerIPC::Code::DrawWindow;
    int rc;
    rc = std::send_message(windowserver_pid, (const char*)&draw_window_code, sizeof(draw_window_code));
    if(rc != E_OK)
    {
        return false;
    }

    rc = std::send_message(windowserver_pid, (const char*)&request, sizeof(request));
    return rc == E_OK;
}

bool recv_draw_request(u32 guid_pid, DrawWindow& request, bool recv_code)
{
    ASSERT(recv_code == false); // not implemented
    u32 tmp_pid = 0;
    u32 size = std::get_message((char*)&request, sizeof(request), tmp_pid);

    return ((size == sizeof(request)) && (tmp_pid == guid_pid));
}

bool send_key_event(u32 gui_pid, const KeyEvent& event)
{
    IOEvent io_event(event);
    u32 event_code = WindowServerIPC::Code::SendIOEvent;

    int rc;
    rc = std::send_message(gui_pid, (const char*)&event_code, sizeof(event_code));
    if(rc != E_OK)
    {
        return false;
    }

    rc = std::send_message(gui_pid, (const char*)&io_event, sizeof(io_event));
    return rc == E_OK;
}

bool send_mouse_event(u32 gui_pid, const MouseEvent& event)
{
    IOEvent io_event(event);
    u32 event_code = WindowServerIPC::Code::SendIOEvent;

    int rc;
    rc = std::send_message(gui_pid, (const char*)&event_code, sizeof(event_code));
    if(rc != E_OK)
    {
        return false;
    }

    rc = std::send_message(gui_pid, (const char*)&io_event, sizeof(io_event));
    return rc == E_OK;
}

bool recv_io_event(u32 windowserver_pid, IOEvent& io_event, bool recv_code)
{
    u32 tmp_pid;
    int rc = 0;
    if(recv_code)
    {
        u32 response_code = 0;
        rc = std::get_message((char*)&response_code, sizeof(response_code), tmp_pid);
        if((rc != sizeof(response_code)) 
            || (response_code != WindowServerIPC::Code::SendIOEvent) 
            || (tmp_pid != windowserver_pid))
        {
            return false;
        }
    }

    const u32 len = std::get_message((char*)&io_event, sizeof(io_event), tmp_pid);
    return ((len==sizeof(io_event)) && (tmp_pid == windowserver_pid));
}

}
