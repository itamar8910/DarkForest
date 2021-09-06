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

}
