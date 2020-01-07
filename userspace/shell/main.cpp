#include "unistd.h"
#include "stdio.h"
#include "asserts.h"
#include "mman.h"
#include "string.h"
#include "malloc.h"
#include "types/vector.h"
#include "types/String.h"
#include "kernel/errs.h"

#include "PS2KeyboardCommon.h"

void process_command(const String& command) {
    auto parts = command.split(' ');
    printf("\n");
    if(parts.size() > 0 && parts[0].len() > 0) {
        auto program = parts[0];
        if(program == "cat") {
            int pid = std::fork_and_exec("/initrd/userspace/cat.app", "cat", parts.range(0, parts.size()));
            ASSERT(pid>0);
            std::wait(pid);
        } 
        if(program == "ls") { // TODO: extract to separate userspace exectuable
            size_t size = 0;
            int rc = std::list_dir(parts[1], nullptr, &size);
            kprintf("rc1:%d\n", rc);
            ASSERT(rc == E_TOO_SMALL);
            Vector<u8> buff(size);
            rc = std::list_dir(parts[1], buff.data(), &size);
            ASSERT(rc == 0);
            

        }
        else {
            printf("program: %s not found\n", program.c_str());
        }
    }
    puts("# ");
}


int main() {
    printf("# ");
    Vector<char> command(50);
    while(1) {
        char c = getchar();
        if(c == '\n') {
            process_command(String(command.data(), command.size()));
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