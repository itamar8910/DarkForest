
#include "df_unistd.h"
#include "asserts.h"
#include "stdio.h"
#include "constants.h"
#include "kernel/errs.h"
#include "sys/socket.h"

int main(char**, size_t) {

    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_ICMP);
    if (fd < 0)
    {
        printf("socket failed\n");
        return 1;
    }
    printf("socket fd: %d\n", fd);
    return 0;
}
