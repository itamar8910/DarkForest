#include "LibGui/Fonts/SimpleFont.h"
#include "LibGui/Widgets/TextBox.h"
#include "LibGui/Widgets/TextView.h"
#include "LibGui/GuiManager.h"
#include "LibGui/Window.h"
#include "df_unistd.h"
#include "stdio.h"

enum class Mode
{
    NORMAL,
    INSERT
};

int main(char* argv[], int argc) {
    Window window = GuiManager::the().create_window(300, 300);
    window.set_background_color(0x33333333);

    TextBox* textbox = new TextBox(20, 20, 280, 250);
    shared_ptr<Widget> textbox_widget(textbox);
    window.add_widget(textbox_widget);

    TextView* textview = new TextView(20, 280, 280, 10);
    shared_ptr<Widget> textview_widget(textview);
    window.add_widget(textview);

    if (argc == 2)
    {
        printf("Got file name : %s\n", argv[1]);
    }

    Mode current_mode = Mode::NORMAL;
    textview->set_text("NORMAL MODE");

    for (;;)
    {
        GuiManager::the().draw(window);

        IOEvent io_event = GuiManager::the().get_io_event(); 
        if(io_event.type != IOEvent::Type::KeyEvent)
        {
            continue;
        }

        KeyEvent key_event = io_event.as_key_event();

        if (key_event.released)
        {
            continue;
        }

        kprintf("vi key event key code: %d\n", key_event.keycode.data);

        u8 keycode = key_event.keycode.data;

        constexpr u8 ESC_KEYCODE = 128;
        if (keycode == ESC_KEYCODE)
        {
            current_mode = Mode::NORMAL;
            textview->set_text("NORMAL MODE");
            continue;
        }

        if (current_mode == Mode::INSERT)
        {
            Point current_cursor_position = textbox->get_cursor_position();

            textbox->set_character((char)keycode, current_cursor_position);

            current_cursor_position.x += 1;

            textbox->set_cursor_position(current_cursor_position);
        }

        else if (keycode == 'q')
        {
            return 0;
        }
        else if (keycode == 'j')
        {
            Point cursor_position = textbox->get_cursor_position();
            cursor_position.y += 1;
            textbox->set_cursor_position(cursor_position);
        }
        else if (keycode == 'k')
        {
            Point cursor_position = textbox->get_cursor_position();
            cursor_position.y -= 1;
            textbox->set_cursor_position(cursor_position);
        }
        else if (keycode == 'l')
        {
            Point cursor_position = textbox->get_cursor_position();
            cursor_position.x += 1;
            textbox->set_cursor_position(cursor_position);
        }
        else if (keycode == 'h')
        {
            Point cursor_position = textbox->get_cursor_position();
            cursor_position.x -= 1;
            textbox->set_cursor_position(cursor_position);
        }
        else if (keycode == '0')
        {
            Point cursor_position = textbox->get_cursor_position();
            cursor_position.x = 0;
            textbox->set_cursor_position(cursor_position);
        }
        else if (keycode == 'i')
        {
            current_mode = Mode::INSERT;
            textview->set_text("INSERT MODE");
        }
    }

    return 0;
}
