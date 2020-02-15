#include "IPC.h"
#include "unistd.h"
#include "kernel/errs.h"

namespace WindowServerIPC
{
bool send_create_window_request(u32 windowserver_pid, CreateWindowRequest& request)
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

bool recv_create_window_response(u32 windowserver_pid, CreateWindowResponse& resp)
{
    u32 response_code = 0;
    u32 tmp_pid;
    int rc = 0;
    rc = std::get_message((char*)&response_code, sizeof(response_code), tmp_pid);
    if(rc != sizeof(response_code))
    {
        return false;
    }
    if(response_code != WindowServerIPC::Code::CreateWindowResponse)
    {
        return false;
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

}