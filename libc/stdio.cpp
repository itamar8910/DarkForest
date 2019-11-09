#include "stdio.h"
#include "syscalls.h"

void puts(const char* str) {
    asm volatile(
        "movl %0, %%ebx\n"
        "movl %1, %%eax\n"
        "int $0x80\n"
        :
        : "rm"(str), "i"(Syscall::Kprintf)
    );
}