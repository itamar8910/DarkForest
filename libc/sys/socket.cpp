#include "socket.h"
#include "syscalls.h"

extern "C"
{
int socket(int domain, int type, int protocol)
{
    return Syscall::invoke(Syscall::Socket, domain, type, protocol);
}
}