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

bool ShellManager::find_app(const String& name, String& app_path)
{
    Vector<Path> PATH;
    PATH.append(Path("/bin"));
    for(auto& path : PATH)
    {
        Path temp_path = path;
        temp_path.add_part(name + ".app");
        if(std::is_file(temp_path.to_string()))
        {
            app_path = temp_path.to_string();
            return true;
        }
    }
    return false;
}

void ShellManager::process_command(const String& command) {
    auto parts = command.split(' ');
    printf("\n");
    if(parts.size() > 0 && parts[0].len() > 0) {
        auto program = parts[0];

        if(program == "cat") {
            int pid = std::fork_and_exec("/root/bin/cat.app", "cat", parts);
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
        else if(program == "vi") {
            int pid = std::fork_and_exec("/root/bin/vi.app", "vi", parts);
            ASSERT(pid>0);
            std::wait(pid);
        }
        else if(program == "touch") {
            do_touch(parts);
            printf("\n");
        }
        else if(program == "mkdir") {
            do_mkdir(parts);
            printf("\n");
        }

        if(program == "cd")
        {
            do_cd(parts);
            printf("\n");
        }
        else
        {
            String app_path;
            bool rc = find_app(program, app_path);
            if(!rc)
            {
                printf("program: %s not found\n", program.c_str());
            }
            else
            {
                int pid = std::fork_and_exec(app_path, program, parts.range(0, parts.size()));
                ASSERT(pid>0);
                std::wait(pid);
            }
            
        }
    }

    print_prompt();
}


void ShellManager::do_cd(const Vector<String>& cmd_parts)
{
    String path = cmd_parts[1];
    if (std::set_current_directory(path) != 0)
    {
        printf("%s is an invalid path\n", path.c_str());
        return;
    }

    if (std::get_current_directory(m_current_directory) != 0)
    {
        printf("GetCurrentDirectory syscall failed :(\n");
    }
}

void ShellManager::print_prompt()
{
    String prompt_string = m_current_directory + String("# ");
    printf("%s", prompt_string.c_str());
}

