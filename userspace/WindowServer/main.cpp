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
#include "WindowServerHandler.h"

void* open_shared_memory(VGA& vga, u32 guid)
{
    void* addr = 0;
    u32 size = 0;
    const int rc = std::open_shared_memory(guid, addr, size);
    kprintf("shell: shared mem: 0x%x\n", addr);
    kprintf("shell: shared mem size: %d\n", size);
    ASSERT(rc == E_OK);
    ASSERT(size == vga.height()*vga.pitch());
    return addr;
}

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
    WindowServerHandler window_server(vga);
    window_server.run();
    return 0;

}
