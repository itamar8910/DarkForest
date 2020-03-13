#include "WindowServerHandler.h"
#include "unistd.h"
#include "LibWindowServer/IPC.h"
#include "stdio.h"
#include "kernel/errs.h"
#include "PS2MouseCommon.h"
#include "Math.h"

WindowServerHandler::WindowServerHandler(VGA& vga) :
    m_vga(vga),
    m_keyboard_fd(std::open("/dev/keyboard")),
    m_mouse_fd(std::open("/dev/mouse")),
    m_windows(),
    m_mouse((m_vga.width()) / 2, (m_vga.height()) / 2)
{
    ASSERT(m_keyboard_fd >=0);
    ASSERT(m_mouse_fd >=0);
    m_vga.clear();
}

void WindowServerHandler::run()
{
    while(true)
    {
        u32 code = 0;
        u32 pid = 0;

        PendingInputBlocker::Reason reason = {};
        u32 ready_fd = 0;
        constexpr u32 NUM_FDS = 2;
        u32 fds[] = {(u32)m_keyboard_fd, (u32)m_mouse_fd};
        const int rc = std::block_until_pending(fds, NUM_FDS, ready_fd, reason);
        ASSERT(rc == E_OK);

        if(reason == PendingInputBlocker::Reason::PendingMessage)
        {
            const int rc = std::get_message((char*)&code, sizeof(u32), pid);
            ASSERT(rc == sizeof(u32));
            handle_message_code(code, pid);
        }
        else if(reason == PendingInputBlocker::Reason::FdReady)
        {
            if(ready_fd == (u32)m_keyboard_fd)
            {
                handle_pending_keyboard_event();
                continue;
            }
            else if(ready_fd == (u32)m_mouse_fd)
            {
                handle_pending_mouse_event();
                continue;
            }
            ASSERT_NOT_REACHED();
        }
    }
}

void WindowServerHandler::handle_pending_keyboard_event()
{
    KeyEvent key_event;
    const int read_rc = std::read(m_keyboard_fd, reinterpret_cast<char*>(&key_event), 1);
    ASSERT(read_rc == 1);

    if(m_current_focused_window_idx == -1)
    {
        return;
    }
    WindowServerIPC::send_key_event(focused_window().owner_pid(), key_event);
}

void WindowServerHandler::handle_pending_mouse_event()
{
    RawMouseEvent event;
    const int read_rc = std::read(m_mouse_fd, reinterpret_cast<char*>(&event), 1);
    ASSERT(read_rc == 1);

    m_mouse.draw(event, m_vga);

    if(m_mouse_state.type == MouseStateType::WindowDrag)
    {
        if(!event.left_button)
        {
            m_mouse_state.set_normal();
            return;
        }
        handle_window_drag(event, m_mouse_state.window_id);
    }
    else if(m_mouse_state.type == MouseStateType::Normal)
    {
        handle_mouse_event(event);
    }

}

void WindowServerHandler::handle_mouse_event(const RawMouseEvent& event)
{
    if(event.left_button)
    {
        u32 window_idx = 0;
        for(auto& window : m_windows)
        {
            if(window.banner_rectangle().intersects(m_mouse.point()))
            {
                set_focused_window(window_idx);
                m_mouse_state.set_drag(window.id());
                draw_window(window);
            }
            if(window.rectangle().intersects(m_mouse.point()))
            {
                set_focused_window(window_idx);
                MouseEvent translated 
                {
                    static_cast<u16>(m_mouse.x() - window.x()),
                    static_cast<u16>(m_mouse.y() - window.y()),
                    event.left_button,
                    event.right_button
                };
                WindowServerIPC::send_mouse_event(window.owner_pid(), translated);
                draw_window(window);
            }

            ++window_idx;
        }
    } 

}
void WindowServerHandler::handle_window_drag(const RawMouseEvent& event, const u32 window_id)
{
    Window window = get_window(m_mouse_state.window_id);

    Window dragged_window(window);
    dragged_window.move_x(event.delta_x);
    dragged_window.move_y(-event.delta_y);
    
    if(!is_in_screen(dragged_window))
    {
        return;
    }

    const u32 window_size = window.width() * (window.height() + window.banner_rectangle().height);
    Vector<u32> background(window_size);
    background.set_size(window_size);
    memset(background.data(), 0xaa, background.size() * sizeof(u32));

    m_vga.draw(
        background.data(),
        window.x(),
        window.y() - window.banner_rectangle().height,
        window.width(),
        window.height() + window.banner_rectangle().height
    );

    set_window(window_id, dragged_window);

    for(auto& w : m_windows)
    {
        draw_window(w);
    }
}

void WindowServerHandler::handle_message_code(u32 code, u32 pid)
{
    switch(code)
    {
       case WindowServerIPC::Code::CreateWindowRequest:
       {
           WindowServerIPC::CreateWindowRequest request;

            bool rc;
            rc = WindowServerIPC::recv_create_window_request(pid, request, false);
            ASSERT(rc);

            Window w(request, pid);

            WindowServerIPC::CreateWindowResponse response = {w.id(), w.buff_guid()};
            rc = WindowServerIPC::send_create_window_response(pid, response);
            ASSERT(rc);

            m_windows.append(w);

            if(m_current_focused_window_idx == -1)
            {
                set_focused_window(m_windows.size() - 1);
            }
            kprintf("focused? %d\n", w.focused());
           break;
       } 

       case WindowServerIPC::Code::DrawWindow:
       {

           WindowServerIPC::DrawWindow request;
           const bool rc = WindowServerIPC::recv_draw_request(pid, request, false);
           ASSERT(rc);

           Window window = get_window(request.window_guid);
           draw_window(window);
           break;
       }

       default:
        {
           ASSERT_NOT_REACHED();
        }
    }
}

Window WindowServerHandler::get_window(u32 window_id)
{
    for(auto& window : m_windows)
    {
        if(window.id() == window_id)
        {
            return window;
        }
    }
    ASSERT_NOT_REACHED();
}

void WindowServerHandler::draw_window(Window& window)
{
    const Rectangle banner_rectangle = window.banner_rectangle();
    const u32 window_banner_size = window.width() * banner_rectangle.height;
    Vector<u32> window_banner(window_banner_size);
    window_banner.set_size(window_banner_size);

    for(u32 i = 0; i < window_banner_size; ++i)
    {
        const u32 banner_color = (window.focused() ? 0x00ff0000 : 0x0000ffff);
        window_banner[i] = banner_color;
    }

    m_vga.draw(window_banner.data(), banner_rectangle.x, banner_rectangle.y,
         banner_rectangle.width,
          banner_rectangle.height);
    m_vga.draw((u32*)window.buff_addr(), window.x(), window.y(), window.width(), window.height());
}

Window& WindowServerHandler::focused_window()
{
    ASSERT(m_current_focused_window_idx >= 0);
    return m_windows[m_current_focused_window_idx];
}

void WindowServerHandler::set_focused_window(u32 index)
{
    if(m_current_focused_window_idx != -1)
    {
        focused_window().set_focused(false);
        draw_window(m_windows[m_current_focused_window_idx]);
    }
    m_current_focused_window_idx = index;
    focused_window().set_focused(true);
}

void WindowServerHandler::MouseState::set_drag(u16 id)
{
    type = MouseStateType::WindowDrag;
    window_id = id;
}
void WindowServerHandler::MouseState::set_normal()
{
    type = MouseStateType::Normal;
    window_id = 0;
}

void WindowServerHandler::set_window(u32 window_id, Window w)
{
    for(auto& window : m_windows)
    {
        if(window.id() == window_id)
        {
            window = w;
            return;
        }
    }
    ASSERT_NOT_REACHED();
}

bool WindowServerHandler::is_in_screen(const Point p)
{
    return p.x >= 0 && p.y >= 0 && (u32)p.x < m_vga.width() && (u32)p.y < m_vga.height();
}

bool WindowServerHandler::is_in_screen(const Window& window)
{
    return is_in_screen({window.x(), window.y() - static_cast<u16>(window.banner_rectangle().height)}) 
           && is_in_screen({window.x() + window.width(), window.y() + window.height()}); 
}