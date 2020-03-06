#include "Vga.h"
#include "types/vector.h"
#include "LibWindowServer/IPC.h"
#include "Window.h"
#include "types/vector.h"
#include "Mouse.h"

class WindowServerHandler final
{
public:
    WindowServerHandler(VGA& vga);
    void run();

private:
    void handle_message_code(u32 code, u32 pid);
    void handle_pending_keyboard_event();
    void handle_pending_mouse_event();
    Window get_window(u32 window_id);

private:
    VGA& m_vga;
    int m_keyboard_fd;
    int m_mouse_fd;
    Vector<Window> m_windows;
    Mouse m_mouse;
};