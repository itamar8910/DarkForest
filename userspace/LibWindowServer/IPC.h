#pragma once
#include "types.h"
#include "PS2KeyboardCommon.h"
#include "PS2MouseCommon.h"
#include "IOEvent.h"

namespace WindowServerIPC
{
    namespace Code
    {
        enum Code_ : u32
        {
            CreateWindowRequest,
            CreateWindowResponse,
            DrawWindow,
            SendIOEvent,
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

bool send_create_window_request(u32 windowserver_pid, const CreateWindowRequest& request);
bool recv_create_window_request(u32 gui_pid, CreateWindowRequest& request, bool recv_code);

bool send_create_window_response(u32 gui_pid, const CreateWindowResponse& resp);
bool recv_create_window_response(u32 windowserver_pid, CreateWindowResponse& resp, bool recv_code);


bool send_draw_request(u32 windowserver_pid, const DrawWindow& request);
bool recv_draw_request(u32 guid_pid, DrawWindow& request, bool recv_code);

bool send_key_event(u32 gui_pid, const KeyEvent& event);
bool send_mouse_event(u32 gui_pid, const MouseEvent& event);
bool recv_io_event(u32 windowserver_pid, IOEvent& request, bool recv_code);
}