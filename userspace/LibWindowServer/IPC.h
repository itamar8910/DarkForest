#pragma once
#include "types.h"

namespace WindowServerIPC
{
    namespace Code
    {
        enum Code_ : u32
        {
            CreateWindowRequest,
            CreateWindowResponse,
            DrawWindow,
        };
    }

struct [[gnu::packed]] CreateWindowRequest
{
    u16 width;
    u16 height;
};

struct [[gnu::packed]] CreateWindowResponse
{
    u32 window_guid;
    u32 buff_guid;
};

struct [[gnu::packed]] DrawWindow
{
    u32 window_guid;
};

bool send_create_window_request(u32 windowserver_pid, CreateWindowRequest& request);
bool recv_create_window_response(u32 windowserver_pid, CreateWindowResponse& resp);

}