#include "df_unistd.h"
#include "stdio.h"
#include "asserts.h"
#include "mman.h"
#include "cstring.h"
#include "malloc.h"
#include "types/vector.h"
#include "types/String.h"
#include "kernel/errs.h"
#include "libc/FileSystem/DirectoryEntry.h"

#include "PS2KeyboardCommon.h"

#include "ShellManager.h"


int main() {

    ShellManager shell;
    Vector<char> command(50);

    while(1) {
        char c = getchar();
        kprintf("shell: getchar: %c\n", c);
        if(c == '\n') {
            String cmd(command.data(), command.size());
            if (cmd == "doom-wad") {
                ASSERT(std::is_file("stuff/doom1.wad"));
                cmd = "doom -iwad stuff/doom1.wad";
            }
            shell.process_command(cmd);

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
