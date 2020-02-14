#include "Vga.h"
#include "types/vector.h"
#include "LibWindowServer/IPC.h"
#include "Window.h"

class WindowServerHandler final
{
public:
    WindowServerHandler(VGA& vga) : m_vga(vga) {}
    void run();

private:
    void handle_message_code(u32 code, u32 pid);

private:
    VGA& m_vga;
    Vector<Window> m_windows = {};
};