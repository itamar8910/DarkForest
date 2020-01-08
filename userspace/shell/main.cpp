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

// TODO: extract to a separate userpsace executable
void do_ls(const Vector<String>& cmd_parts)
{
    size_t size = 0;
    int rc = std::list_dir(cmd_parts[1], nullptr, &size);
    kprintf("rc1:%d\n", rc);
    if(rc == E_NOTFOUND)
    {
        printf("directory not found\n");
        return;
    }
    ASSERT(rc == E_TOO_SMALL);
    Vector<u8> buff(size);
    rc = std::list_dir(cmd_parts[1], buff.data(), &size);
    ASSERT(rc == 0);
    print_hexdump(buff.data(), size);
    size_t offset = 0;
    Vector<DirectoryEntry> entries;
    while(offset < size)
    {
        kprintf("offset: %d\n", offset);
        size_t entry_size = 0;
        entries.append(DirectoryEntry::deserialize(buff.data() + offset, entry_size));
        offset += entry_size;
    }

    for(auto& entry : entries)
    {
        printf("%s%s  ", entry.path.to_string().c_str(), entry.type == DirectoryEntry::Type::Directory ? "/" : "");
    }
}

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
            do_ls(parts);
            printf("\n");
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