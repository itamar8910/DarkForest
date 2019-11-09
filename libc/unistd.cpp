#include "unistd.h"
#include "syscalls.h"

void exit(int status) {
    asm volatile(
        "movl %0, %%ebx\n"
        "movl %1, %%eax\n"
        "int $0x80\n"
        :
        : "rm"(status), "i"(Syscall::Exit)
    );
}