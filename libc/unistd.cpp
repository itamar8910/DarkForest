#include "unistd.h"
#include "df_unistd.h"
#include "syscalls.h"

extern "C" {

void exit(int status)
{
    std::exit(status);
}

off_t lseek(int fd, off_t offset, int whence)
{
    return Syscall::invoke(Syscall::Lseek, fd, offset, whence);
}

u32 df_time_since_boot_ms()
{
    return Syscall::invoke(Syscall::TimeSinceBootMs);
}

void df_sleep_ms(size_t ms)
{
    std::sleep_ms(ms);
}

}
