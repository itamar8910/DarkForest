#include "unistd.h"
#include "stdio.h"
#include "asserts.h"
#include "mman.h"
#include "string.h"
#include "malloc.h"
#include "types/vector.h"
#include "types/String.h"
#include "kernel/errs.h"
#include "asserts.h"
#include "LibWindowServer/IPC.h"
#include "LibGui/GuiManager.h"
#include "shared_ptr.h"
#include "LibGui/Widgets/TextView.h"

int main() {
    printf("gui!\n");
    std::sleep_ms(1000);

    Window window = GuiManager::the().create_window(300, 400);

    TextView* tv = new TextView(20,20,200,200);
    tv->set_text("Hello World!");
    shared_ptr<Widget> text_view(tv);

    window.add_widget(text_view);

    for(size_t i = 0;;++i)
    {

        // window.set_background_color(0xdeadbeef*(i+1));

        GuiManager::the().draw(window);

        KeyEvent key_event = GuiManager::the().get_keyboard_event(); 
        if(!key_event.released)
        {
            continue;
        }

        kprintf("gui: key event: %c\n", key_event.to_ascii());
        tv->set_text(tv->get_text() + String(key_event.to_ascii()));

    }

    return 0;
}
