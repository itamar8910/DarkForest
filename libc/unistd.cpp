#include "types/vector.h"
#include "kernel/errs.h"
#include "unistd.h"
#include "syscalls.h"
#include "shared_ptr.h"
#include "fork_args.h"

namespace std
{

[[noreturn]] void exit(int status) {
    Syscall::invoke(Syscall::Exit, status);
    for(;;);
}

void sleep_ms(u32 ms) {
    Syscall::invoke(Syscall::SleepMs, ms);
}

int open(const char* path) {
    return Syscall::invoke(Syscall::Open, (u32)path);
}

int ioctl(int fd, u32 code, void* data) {
    return Syscall::invoke(Syscall::IOCTL, fd, code, (u32)data);
}

int file_size(int fd) {
    return Syscall::invoke(Syscall::FILE_SIZE, fd);
}

int read(size_t fd, char* buff, size_t count) {
    return Syscall::invoke(Syscall::READ, fd, (u32)buff, count);
}

int write(size_t fd, const char* buff, size_t count) {
    return Syscall::invoke(Syscall::WRITE, fd, (u32)buff, count);
}

int fork_and_exec(const String& path, const String& name, const Vector<String>& args)
{
    char** argv = nullptr;
    size_t argc = 0;
    if(args.size() != 0){
        argc = args.size();
        argv = new char*[args.size()];
        for(size_t i = 0; i < args.size(); ++i){
            argv[i] = new char[args.at(i).len() + 1];
            strcpy(argv[i], args.at(i).c_str());
        }
    } 
    ForkArgs fork_args = {path.c_str(), name.c_str(), argv, argc};
    int rc =  Syscall::invoke(Syscall::ForkAndExec,
                                (u32) &fork_args
                            );
    if(argv != nullptr) {
        for(size_t i = 0; i < args.size(); ++i){
            delete[] argv[i];
        }
        delete[] argv;
    }
    return rc;
}

int wait(size_t pid)
{
    return Syscall::invoke(Syscall::WAIT, pid);
}

int list_dir(const String& path, void* dest, size_t* size)
{
    return Syscall::invoke(Syscall::ListDir, (u32) path.c_str(), (u32) dest, (u32) size);
}

int set_current_directory(const String& path)
{
    return Syscall::invoke(Syscall::SetCurrentDirectory, (u32) path.c_str());
}

int get_current_directory(String& out_path)
{
    size_t required_count = 0;
    int syscall_return_code = 0;

    syscall_return_code = Syscall::invoke(Syscall::GetCurrentDirectory, (u32) nullptr, (u32) &required_count);
    if (syscall_return_code != E_TOO_SMALL)
    {
        return syscall_return_code;
    }

    Vector<char> current_directory_vec(required_count);
    syscall_return_code = Syscall::invoke(Syscall::GetCurrentDirectory, (u32) current_directory_vec.data(), (u32) &required_count);
    if (syscall_return_code != 0)
    {
        return syscall_return_code;
    }
    
    out_path = String(current_directory_vec.data(), required_count);

    return syscall_return_code;
}

int create_file(const String& path)
{
    return Syscall::invoke(Syscall::CreateFile, (u32) path.c_str());
}

int create_directory(const String& path)
{
    return Syscall::invoke(Syscall::CreateDirectory, (u32) path.c_str());
}

}
