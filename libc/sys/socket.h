#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define PF_INET 2
#define SOCK_DGRAM 2
#define IPPROTO_ICMP 1

int socket(int domain, int type, int protocol);

#ifdef __cplusplus
}
#endif
