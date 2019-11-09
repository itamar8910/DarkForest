#include "unistd.h"
#include "syscalls.h"

[[noreturn]] void exit(int status) {
    asm volatile(
        // "pushl %%ebx\n" // ebx is callee saved
        "movl %0, %%ebx\n"
        "movl %1, %%eax\n"
        "int $0x80\n"
        // "popl %%ebx\n"
        :
        : "rm"(status), "i"(Syscall::Exit)
        : "ebx", "eax"
    );
    for(;;);
}

void sleep_ms(u32 ms) {
    asm volatile(
        // "pushl %%ebx\n" // ebx is callee saved
        "movl %0, %%ebx\n"
        "movl %1, %%eax\n"
        "int $0x80\n"
        // "popl %%ebx\n"
        :
        : "rm"(ms), "i"(Syscall::SleepMs)
        : "ebx", "eax"
    );

}