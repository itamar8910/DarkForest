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
#include "LibGui/Widgets/TextView.h"

int main() {
    printf("gui!\n");
    std::sleep_ms(1200);

    Window window = GuiManager::the().create_window(100, 200);
    TextView* tv = new TextView(20,20,80,20);
    tv->set_text("Click me");
    shared_ptr<Widget> text_view(tv);
    window.add_widget(text_view);

    for(size_t i = 0;;++i)
    {
        
        window.set_background_color(0xc0012eef*(i+1));

        GuiManager::the().draw(window);

        IOEvent io_event = GuiManager::the().get_io_event(); 
        if(io_event.type != IOEvent::Type::MouseEvent)
        {
            continue;
        }

        MouseEvent event = io_event.as_mouse_event();
        if(!event.left_button)
        {
            continue;
        }
    }

    return 0;
}
