#include "socket.h"
#include "syscalls.h"

extern "C"
{
int socket(int domain, int type, int protocol)
{
    return Syscall::invoke(Syscall::Socket, domain, type, protocol);
}

int sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr_in *dest_addr, size_t addrlen)
{
    SendToArgs args{
        .sockfd = sockfd,
        .buf=buf,
        .len=len,
        .flags=flags,
        .dest_addr=dest_addr,
        .addrlen=addrlen
    };
    return Syscall::invoke(Syscall::SendTo, (u32)&args);
}

int recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr_in *dest_addr, size_t* addrlen)
{
    RecvFromArgs args{
        .sockfd = sockfd,
        .buf=buf,
        .len=len,
        .flags=flags,
        .dest_addr=dest_addr,
        .addrlen=addrlen
    };
    return Syscall::invoke(Syscall::RecvFrom, (u32)&args);
}

}