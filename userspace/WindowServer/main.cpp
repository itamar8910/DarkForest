#include "unistd.h"
#include "stdio.h"
#include "asserts.h"
#include "mman.h"
#include "string.h"
#include "malloc.h"
#include "types/vector.h"
#include "types/String.h"
#include "kernel/errs.h"
#include "Vga.h"

constexpr u32 SHM_GUID = 1;

// void try_shared_mem()
// {
//     void* addr = 0;
//     const int rc = std::create_shared_memory(SHM_GUID, 4096, addr);
//     kprintf("shell: shared mem: 0x%x\n", addr);
//     reinterpret_cast<char*>(addr)[0] = 'a';
//     ASSERT(rc == E_OK);
// }

// void query_shared_mem()
// {
//     void* addr = 0;
//     u32 size = 0;
//     const int rc = std::open_shared_memory(SHM_GUID, addr, size);
//     ASSERT(rc == E_OK);
//     kprintf("shell: shared mem: 0x%x\n", addr);
//     kprintf("shell: shared mem char: %d\n", reinterpret_cast<char*>(addr)[0]);
// }

// void try_messages()
// {
//     u32 msg;
//     kprintf("getting message...\n");
//     const int rc = std::get_message(msg);
//     ASSERT(rc == E_OK);
//     kprintf("shell: msg: 0x%x\n", msg);
// }

bool already_running()
{
    u32 pid = 0;
    const int rc = std::get_pid_by_name("WindowServer", pid);
    return (rc == E_OK);
}

int main() {
    printf("WindowServer!\n");
    if(already_running())
    {
        printf("WindowServer is already running\n");
        return 1;
    }

    VGA vga;

    u32* frame_buffer = vga.frame_buffer();

    for(size_t row = 0; row < vga.height(); ++row)
    {
        for(size_t col = 0; col < vga.width(); ++col)
        {
            u32* pixel = frame_buffer + (row*vga.width() + col);
            *pixel = (col%10 < 5) ? 0xFF0000FF : 0xFF00FF00;
        }
    }
    while(true)
    {
        u32 msg;
        const int rc = std::get_message(msg);
        ASSERT(rc == E_OK);
        kprintf("WindowServer got message: 0x%x\n", msg);
    }
    return 0;
}
