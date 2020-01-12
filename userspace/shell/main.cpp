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

int main() {
    kprintf("a1\n");
    ShellManager shell;
    Vector<char> command(50);
    kprintf("a2\n");

    while(1) {
        kprintf("a3\n");
        char c = getchar();
        kprintf("a4\n");
        if(c == '\n') {
            shell.process_command(String(command.data(), command.size()));
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
