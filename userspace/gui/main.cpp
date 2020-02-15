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

    u32* frame_buffer = (u32*)window.buff_addr();
    for(size_t i = 0;;++i)
    {
        memset(window.buff_addr(), 0, window.buff_size());
        for(size_t row = 0; row < window.height(); ++row)
        {
            for(size_t col = 0; col < window.width(); ++col)
            {
                u32* pixel = frame_buffer + (row*window.width() + col);
                *pixel = (0xdeadbeef*(i+1));
            }
        }

        GuiManager::the().draw(window);

        std::sleep_ms(500);
    }

    return 0;
}
