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

void sleep_ms(u32 ms) {
    asm volatile(
        "movl %0, %%ebx\n"
        "movl %1, %%eax\n"
        "int $0x80\n"
        :
        : "rm"(ms), "i"(Syscall::SleepMs)
    );

}