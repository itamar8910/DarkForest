#include "types/vector.h"
#include "stdio.h"
#include "kernel/errs.h"
#include "syscalls.h"
#include "shared_ptr.h"
#include "fork_args.h"
#include "ioctl_common.h"
#include "df_unistd.h"

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
    return Syscall::invoke(Syscall::IOctl, fd, code, (u32)data);
}

int file_size(int fd) {
    return Syscall::invoke(Syscall::FileSize, fd);
}

int read(size_t fd, char* buff, size_t count) {
    return Syscall::invoke(Syscall::Read, fd, (u32)buff, count);
}

int write(size_t fd, const char* buff, size_t count) {
    return Syscall::invoke(Syscall::Write, fd, (u32)buff, count);
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
    return Syscall::invoke(Syscall::Wait, pid);
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

int is_file(const String& path)
{
    return Syscall::invoke(Syscall::IsFile, (u32) path.c_str());
}

int is_directory(const String& path)
{
    return Syscall::invoke(Syscall::IsDirectory, (u32) path.c_str());
}

int create_shared_memory(u32 guid, u32 size, void*& addr)
{
    void* tmp_addr = 0;
    const int rc = Syscall::invoke(Syscall::CreateSharedMemory, guid, size, reinterpret_cast<u32>(&tmp_addr));
    if(rc != E_OK)
    {
        return rc;
    }
    addr = tmp_addr;
    return E_OK;
}

int open_shared_memory(u32 guid, void*& addr, u32& size)
{
    void* tmp_addr = 0;
    u32 tmp_size = 0;
    const int rc = Syscall::invoke(Syscall::OpenSharedMemory, guid, reinterpret_cast<u32>(&tmp_addr), reinterpret_cast<u32>(&tmp_size));
    if(rc != E_OK)
    {
        return rc;
    }
    addr = tmp_addr;
    size = tmp_size;
    return E_OK;
}

int send_message(u32 pid, const char* msg, u32 size)
{
    return Syscall::invoke(Syscall::SendMessage, pid, (u32)msg, (u32)size);
}

int get_message(char* msg, u32 size, u32& pid)
{
    u32 tmp_pid = 0;
    const int rc = Syscall::invoke(Syscall::GetMessage, (u32)(msg), (u32)size, (u32)&tmp_pid);
    pid = tmp_pid;
    return rc;
}

int get_pid_by_name(const String& name, u32& pid)
{
    u32 tmp_pid = 0;
    const int rc = Syscall::invoke(Syscall::GetPidByName, reinterpret_cast<u32>(name.c_str()), reinterpret_cast<u32>(&tmp_pid));
    if(rc != E_OK)
    {
        return rc;
    }

    pid = tmp_pid;
    return E_OK;

}

int map_device(int fd, void* addr, u32 size)
{
    return Syscall::invoke(Syscall::MapDevice, static_cast<u32>(fd), (u32)addr, size);
}

u32 generate_guid()
{
    return (u32) Syscall::invoke(Syscall::GenerateGUID);
}

int block_until_pending(u32* fds, u32 num_fds, u32& ready_fd, PendingInputBlocker::Reason& reason)
{
    u32 tmp_ready_fd = 0;
    const int rc = Syscall::invoke(Syscall::BlockUntilPending, reinterpret_cast<u32>(fds), (u32)num_fds, reinterpret_cast<u32>(&tmp_ready_fd));
    if(rc >= 0)
    {
        reason = static_cast<PendingInputBlocker::Reason>(rc);
        ready_fd = tmp_ready_fd; 
        return E_OK;
    }
    return -rc;
}

int create_terminal(char* name_out)
{
    return Syscall::invoke(Syscall::CreateTerminal, reinterpret_cast<u32>(name_out));
}

int has_pending_message()
{
    return Syscall::invoke(Syscall::HasPendingMessage);
}

}
