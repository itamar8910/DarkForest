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

int main() {
    printf("gui!\n");
    std::sleep_ms(1000);

    Window window = GuiManager::the().create_window(100, 200);

    for(size_t i = 0;;++i)
    {
        
        window.set_background_color(0xdeadbeef*(i+1));

        GuiManager::the().draw(window);

        std::sleep_ms(500);
    }

    return 0;
}
