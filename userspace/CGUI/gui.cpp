#include "gui.h"

#include "LibWindowServer/IPC.h"
#include "LibGui/GuiManager.h"
#include "LibGui/Widgets/TextView.h"
#include "df_unistd.h"

extern "C" {

DF_WINDOW cgui_create_window(u16 width, u16 height)
{
    Window* window = new Window(GuiManager::the().create_window(width, height));
    window->set_background_color(0x66666666);
    window->set_raw_mode(true);
    return window;
}

void cgui_draw_window(DF_WINDOW window_handle, uint32_t* buffer)
{
    (void)buffer;
    Window* window = (Window*)window_handle;
    window->copy_to_buffer(buffer);
    // TODO: set framebuffer
    GuiManager::the().draw(*window);
}

int cgui_get_key_event(CGUI_KeyEvent* out_event)
{
    if (!std::has_pending_message())
        return false;

    IOEvent io_event = GuiManager::the().get_io_event(); 
    if (io_event.type != IOEvent::Type::KeyEvent)
        return false;

    KeyEvent key_event = io_event.as_key_event();
    out_event->pressed = key_event.released ? 0 : 1;
    out_event->key = key_event.keycode.data;
    return true;
}

}
