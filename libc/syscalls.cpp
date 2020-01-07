#include "syscalls.h"
#include "types.h"

int Syscall::invoke(SyscallIdx syscall, u32 arg1, u32 arg2, u32 arg3) {
    int ret;
    asm volatile(
        "movl %1, %%ebx\n"
        "movl %2, %%ecx\n"
        "movl %3, %%edx\n"
        "movl %4, %%eax\n"
        "int $0x80\n"
        "movl %%eax, %0\n"
        : "=r"(ret)
        : "rm"(arg1),
          "rm"(arg2),
          "rm"(arg3),
          "rm"((u32)syscall)
        : "ebx", "eax", "ecx", "edx"
    );
    return ret;
} 