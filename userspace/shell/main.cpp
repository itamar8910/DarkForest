#include "unistd.h"
#include "stdio.h"
#include "asserts.h"
#include "mman.h"
#include "string.h"
#include "malloc.h"
#include "types/vector.h"
#include "types/String.h"
#include "kernel/errs.h"
#include "libc/FileSystem/DirectoryEntry.h"

#include "PS2KeyboardCommon.h"

#include "ShellManager.h"

constexpr u32 SHM_GUID = 1;

void try_shared_mem()
{
    void* addr = 0;
    const int rc = std::create_shared_memory(SHM_GUID, 4096, addr);
    kprintf("shell: shared mem: 0x%x\n", addr);
    reinterpret_cast<char*>(addr)[0] = 'a';
    ASSERT(rc == E_OK);
}

void query_shared_mem()
{
    void* addr = 0;
    u32 size = 0;
    const int rc = std::open_shared_memory(SHM_GUID, addr, size);
    ASSERT(rc == E_OK);
    kprintf("shell: shared mem: 0x%x\n", addr);
    kprintf("shell: shared mem char: %d\n", reinterpret_cast<char*>(addr)[0]);
}

void try_messages()
{
    u32 msg;
    kprintf("getting message...\n");
    const int rc = std::get_message(msg);
    ASSERT(rc == E_OK);
    kprintf("shell: msg: 0x%x\n", msg);
}

int main() {

    try_shared_mem();

    ShellManager shell;
    Vector<char> command(50);

    while(1) {
        char c = getchar();
        if(c == '\n') {
            shell.process_command(String(command.data(), command.size()));

            query_shared_mem();
            try_messages();

            command.clear();
        }
		else {
            if(c == '\b') { // backspace
                if(command.size() == 0)
                    continue;
                command.pop();
            } else {
                command.append(c);
            }
            putc(c);
		}
    }
    return 0;
}
