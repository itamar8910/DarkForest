#include "unistd.h"
#include "stdio.h"
#include "asserts.h"
#include "libc/FileSystem/DirectoryEntry.h"
#include "kernel/errs.h"
#include "ShellManager.h"

ShellManager::ShellManager() :
    m_current_directory("/") {
        print_prompt();
}

void ShellManager::process_command(const String& command) {
    auto parts = command.split(' ');
    printf("\n");
    if(parts.size() > 0 && parts[0].len() > 0) {
        auto program = parts[0];
        if(program == "cat") {
            int pid = std::fork_and_exec("/root/bin/cat.app", "cat", parts.range(0, parts.size()));
            ASSERT(pid>0);
            std::wait(pid);
        } 
        else if(program == "ls") { // TODO: extract to separate userspace exectuable
            do_ls(parts);
            printf("\n");
        }
        else if(program == "echo") { // TODO: extract to separate userspace exectuable
            do_echo(parts);
            printf("\n");
        }
        else if(program == "cd") {
            do_cd(parts);
            printf("\n");
        }
        else {
            printf("program: %s not found\n", program.c_str());
        }
    }

    print_prompt();
}

// TODO: extract to a separate userpsace executable
void ShellManager::do_ls(const Vector<String>& cmd_parts)
{
    if(cmd_parts.size() > 2)
    {
        printf("usage: ls [directory_path]\n");
        return;
    }

    String directory = m_current_directory;

    if(cmd_parts.size() == 2)
    {
        directory = cmd_parts[1];
    }

    size_t size = 0;
    int rc = std::list_dir(directory, nullptr, &size);
    kprintf("rc1:%d\n", rc);
    if(rc == E_NOTFOUND)
    {
        printf("directory not found\n");
        return;
    }
    ASSERT(rc == E_TOO_SMALL);
    Vector<u8> buff(size);
    rc = std::list_dir(directory, buff.data(), &size);
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
        printf("%s%s  ", entry.path().to_string().c_str(), entry.type() == DirectoryEntry::Type::Directory ? "/" : "");
    }
}

void ShellManager::do_echo(const Vector<String>& cmd_parts)
{
    // echo hello > a.txt
    if(
        (cmd_parts.size() != 4)
        || (cmd_parts[2] != ">")
    )
    {
        kprintf("cmd_parts.size(): %d\n", cmd_parts.size());
        kprintf("cmd_parts[2]: %s\n", cmd_parts[2].c_str());
        printf("usage: echo [text] > [path to file]\n");
        return;
    }
    String text = cmd_parts[1] + "\n";
    String path = cmd_parts[3];

    int fd = std::open(path.c_str());
    if(fd < 0) {
        printf("error opening file: %s\n", path.c_str());
        return;
    }
    int rc = std::write(fd, text.c_str(), text.len());
    if(rc < 0)
    {
        printf("error in write: %d\n", rc);
    }
}

void ShellManager::do_cd(const Vector<String>& cmd_parts)
{
    String path = cmd_parts[1];
    std::set_current_directory(path);
    m_current_directory = path;
}

void ShellManager::print_prompt()
{
    String prompt_string = m_current_directory + String("# ");
    printf("%s", prompt_string.c_str());
}

