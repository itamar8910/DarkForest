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
    void set_window(u32 window_id, Window w);
    void draw_window(Window& window);

    Window& focused_window();
    void set_focused_window(u32 index);

    void handle_mouse_event(const RawMouseEvent& event);
    void handle_window_drag(const RawMouseEvent& event, const u32 window_id);

    bool is_in_screen(const Point p);
    bool is_in_screen(const Window& window);

    enum class MouseStateType
    {
        Normal,
        WindowDrag,
    };

    struct MouseState
    {
        MouseStateType type;
        union {
            u32 window_id;
        };
        
        void set_drag(u16 id);
        void set_normal();
    };


private:
    VGA& m_vga;
    int m_keyboard_fd;
    int m_mouse_fd;
    Vector<Window> m_windows;
    Mouse m_mouse;
    int m_current_focused_window_idx {-1};
    MouseState m_mouse_state {MouseStateType::Normal, 0};
};
