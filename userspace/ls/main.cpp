
#include "unistd.h"
#include "asserts.h"
#include "stdio.h"
#include "constants.h"
#include "libc/FileSystem/DirectoryEntry.h"
#include "kernel/errs.h"

void do_ls(const String& directory) {
    size_t size = 0;
    int rc = std::list_dir(directory, nullptr, &size);
    if(rc == E_NOTFOUND)
    {
        printf("directory not found\n");
        return;
    }
    ASSERT(rc == E_TOO_SMALL);
    Vector<u8> buff(size);
    rc = std::list_dir(directory, buff.data(), &size);
    ASSERT(rc == 0);
    size_t offset = 0;
    Vector<DirectoryEntry> entries;
    while(offset < size)
    {
        size_t entry_size = 0;
        entries.append(DirectoryEntry::deserialize(buff.data() + offset, entry_size));
        offset += entry_size;
    }

    for(auto& entry : entries)
    {
        printf("%s%s  ", entry.path().to_string().c_str(), entry.type() == DirectoryEntry::Type::Directory ? "/" : "");
    }
}

void try_shared_mem()
{
    void* addr = 0;
    u32 size = 0;
    const int rc = std::open_shared_memory(1, addr, size);
    kprintf("ls: shared mem: 0x%x, %d\n", addr, size);
    ASSERT(rc == E_OK);
    const char c = reinterpret_cast<char*>(addr)[0];
    kprintf("ls: shared mem char: %d\n", c);
    reinterpret_cast<char*>(addr)[0] = 'b';
}

void try_messages()
{
    u32 pid = 0;
    const int rc = std::get_pid_by_name("shell", pid);
    ASSERT(rc == E_OK);
    kprintf("shell pid: %d\n", pid);
    const int msg_rc = std::send_message(pid, 0xdeadbeab);
    ASSERT(msg_rc == E_OK);
}

int main(char** argv, size_t argc) {
    try_shared_mem();
    try_messages();

    if(argc < 1) {
        printf("expected argc >= 1\n");
        return 1;
    }
    if(argc > 2) {
        printf("Usage: %s [directory_path]\n", argv[0]);
        return 1;
    }
    String directory = ".";

    if(argc == 2)
    {
        directory = argv[1];
    }

    do_ls(directory);

    printf("\n");
    return 0;
}