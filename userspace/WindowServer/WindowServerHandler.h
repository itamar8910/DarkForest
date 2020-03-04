#include "Vga.h"
#include "types/vector.h"
#include "LibWindowServer/IPC.h"
#include "Window.h"
#include "types/vector.h"

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
    static void init_mouse_sprite();

private:
    VGA& m_vga;
    int m_keyboard_fd;
    int m_mouse_fd;
    int m_mouseX {0};
    int m_mouseY {0};
    Vector<Window> m_windows;

    static constexpr u32 MOUSE_SPRITE_SIZE = 7;
    static u32 mouse_sprite_template[MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE];
    static u32 current_mouse_sprite[MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE];
    Vector<u32> m_hidden_by_mouse;
};