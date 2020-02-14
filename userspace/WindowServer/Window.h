#include "types.h"
#include "LibWindowServer/IPC.h"

class Window final
{
public:
    Window(const WindowServerIPC::CreateWindowRequest& request);
    u32 m_id = {0};
    u32 m_buff_guid = {0};
    void* m_buff_addr = {nullptr};
    u16 m_x = {0};
    u16 m_y = {0};
    u16 m_width = {0};
    u16 m_height = {0};
};