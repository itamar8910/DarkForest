#include "types.h"
#include "LibWindowServer/IPC.h"
#include "Geometry.h"

class Window final
{
public:
    Window(const WindowServerIPC::CreateWindowRequest& request, u32 pid);
    Window() = default;

    u32 id() const {return m_id;}
    u32 owner_pid() const {return m_owner_pid;}
    u32 buff_guid() const {return m_buff_guid;}
    void* buff_addr() const {return m_buff_addr;}
    u32 buff_size() const {return m_buff_size;}
    u16 x() const {return m_x;}
    u16 y() const {return m_y;}
    u32 width() const {return m_width;}
    u32 height() const {return m_height;}
    Rectangle rectangle() const;
    void set_focused(bool focused) {m_focused = focused;}
    bool focused() const {return m_focused;}

private:
    u32 m_id = {0};
    u32 m_owner_pid;
    u32 m_buff_guid = {0};
    void* m_buff_addr = {nullptr};
    u32 m_buff_size = {0};
    u16 m_x = {0};
    u16 m_y = {0};
    u16 m_width = {0};
    u16 m_height = {0};
    bool m_focused = {false};
};