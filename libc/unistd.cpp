#include "unistd.h"
#include "syscalls.h"

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