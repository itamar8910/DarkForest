#pragma once
#include "types.h"
#include "types/String.h"
#include "types/vector.h"

#define CODE_ASSERT_NOT_REACHED 127

namespace std
{
[[noreturn]] void exit(int status);

void sleep_ms(u32 ms);

int open(const char* path);
int ioctl(int fd, u32 code, void* data);
int file_size(int fd);
int read(size_t fd, char* buff, size_t count);
int write(size_t fd, const char* buff, size_t count);

// executes a new process from executable at 'path',
// with a copy of the calling process's file descriptor table
int fork_and_exec(const String& path,
                    const String& name,
                    const Vector<String>& args = Vector<String>()
                    );
int wait(size_t pid);

int list_dir(const String& path, void* dest, size_t* size);

int set_current_directory(const String& path);
int get_current_directory(String& out_path);

int create_file(const String& path);
int create_directory(const String& path);

int is_file(const String& path);
int is_directory(const String& path);

int create_shared_memory(u32 guid, u32 size, void*& addr);
int open_shared_memory(u32 guid, void*& addr, u32& size);

int send_message(u32 pid, u32 msg);
int get_message(u32& msg);

int get_pid_by_name(const String& name, u32& pid);

int map_device(int fd, void* addr, u32 size);

}
