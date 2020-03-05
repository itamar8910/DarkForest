#include "LibGui/Fonts/SimpleFont.h"
#include "LibGui/Widgets/TextBox.h"
#include "LibGui/Widgets/TextView.h"
#include "LibGui/GuiManager.h"
#include "LibGui/Window.h"
#include "unistd.h"
#include "stdio.h"

enum class Mode
{
    NORMAL,
    INSERT
};

int main(char* argv[], int argc) {
    Window window = GuiManager::the().create_window(300, 300);
    window.set_background_color(0x33333333);

    TextBox* textbox = new TextBox(10, 10, 280, 280);
    shared_ptr<Widget> widget(textbox);
    window.add_widget(widget);

    if (argc == 2)
    {
        printf("Got file name : %s\n", argv[1]);
    }

    Mode current_mode = Mode::NORMAL;

    for (;;)
    {
        GuiManager::the().draw(window);

        KeyEvent key_event = GuiManager::the().get_keyboard_event();

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
            continue;
        }

        if (current_mode == Mode::INSERT)
        {
            Point current_cursor_position = textbox->get_cursor_position();

            textbox->set_character((char)keycode, current_cursor_position);

            current_cursor_position.x += SIMPLEFONT_SYMBOL_SIZE + 2;

            textbox->set_cursor_position(current_cursor_position);
        }

        if (keycode == 'q')
        {
            return 0;
        }
        else if (keycode == 'j')
        {
            Point cursor_position = textbox->get_cursor_position();
            cursor_position.y += SIMPLEFONT_SYMBOL_SIZE + 2;
            textbox->set_cursor_position(cursor_position);
        }
        else if (keycode == 'k')
        {
            Point cursor_position = textbox->get_cursor_position();
            cursor_position.y -= SIMPLEFONT_SYMBOL_SIZE + 2;
            textbox->set_cursor_position(cursor_position);
        }
        else if (keycode == 'l')
        {
            Point cursor_position = textbox->get_cursor_position();
            cursor_position.x += SIMPLEFONT_SYMBOL_SIZE + 2;
            textbox->set_cursor_position(cursor_position);
        }
        else if (keycode == 'h')
        {
            Point cursor_position = textbox->get_cursor_position();
            cursor_position.x -= SIMPLEFONT_SYMBOL_SIZE + 2;
            textbox->set_cursor_position(cursor_position);
        }
        else if (keycode == 'i')
        {
            current_mode = Mode::INSERT;
        }
    }

    return 0;
}
