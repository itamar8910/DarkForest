#include "Vga.h"
#include "types/vector.h"
#include "LibWindowServer/IPC.h"
#include "Window.h"

class WindowServerHandler final
{
public:
    WindowServerHandler(VGA& vga) : m_vga(vga), m_windows() {}
    void run();

private:
    void handle_message_code(u32 code, u32 pid);
    Window get_window(u32 window_id);

private:
    VGA& m_vga;
    Vector<Window> m_windows;
};