#include "syscalls.h"
#include "types.h"

// TODO: when we'll have syscalls with multiple args,
// just put arg0=0, arg1=0,.. (i.e default paramter values)
// in this function
int Syscall::invoke(SyscallIdx syscall, u32 arg) {
    asm volatile(
        "movl %0, %%ebx\n"
        "movl %1, %%eax\n"
        "int $0x80\n"
        :
        : "rm"(arg), "rm"((u32)syscall)
        : "ebx", "eax"
    );
    // TODO: get actual return value of syscall
    return 0;
} 