#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PF_INET 2
#define SOCK_DGRAM 2
#define IPPROTO_ICMP 1

typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;
typedef int sa_family_t;

struct in_addr
{
    in_addr_t s_addr;
};

struct sockaddr_in
{
    sa_family_t sin_family;
    in_port_t  sin_port;
    struct in_addr sin_addr;
};

struct SendToArgs
{
    int sockfd;
    const void *buf;
    size_t len;
    int flags;
    const struct sockaddr_in *dest_addr;
    size_t addrlen;
};

struct RecvFromArgs
{
    int sockfd;
    void *buf;
    size_t len;
    int flags;
    struct sockaddr_in *dest_addr;
    size_t* addrlen;
};

int socket(int domain, int type, int protocol);

int sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr_in *dest_addr, size_t addrlen);
int recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr_in *dest_addr, size_t* addrlen);


#ifdef __cplusplus
}
#endif
