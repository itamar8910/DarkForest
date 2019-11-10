#include "unistd.h"
#include "syscalls.h"

[[noreturn]] void exit(int status) {
    Syscall::invoke(Syscall::Exit, status);
    for(;;);
}

void sleep_ms(u32 ms) {
    Syscall::invoke(Syscall::SleepMs, ms);
}