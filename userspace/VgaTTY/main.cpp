#include "unistd.h"
#include "stdio.h"
#include "asserts.h"
#include "mman.h"
#include "string.h"
#include "malloc.h"

#include "ioctl_common.h"
#include "VgaTextCommon.h"

int main() {
    printf("hello from: %s\n", "userspace!");
    int fd = open("/dev/vgatext");
    printf("vgatext fd: %d\n", fd);
    IOCTL::VgaText::Data ioctl_data {
            0,
            0,
            VgaTextCommon::compose_entry('X',
            VgaTextCommon::compose_color(
                VgaTextCommon::VgaColor::VGA_COLOR_LIGHT_GREY,
                VgaTextCommon::VgaColor::VGA_COLOR_BLACK)
            )
        };
    ioctl(fd, static_cast<size_t>(IOCTL::VgaText::Code::PUT_CHAR), &ioctl_data);
    
    return 0;
}