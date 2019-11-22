#include "syscalls.h"
#include "types.h"

// TODO: when we'll have syscalls with multiple args,
// just put arg0=0, arg1=0,.. (i.e default paramter values)
// in this function
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
        : "ebx", "eax"
    );
    return ret;
} 