#include "stdio.h"
#include "syscalls.h"
#include "stdarg.h"
#include "printf.h"

void puts(const char* str) {
    asm volatile(
        "movl %0, %%ebx\n"
        "movl %1, %%eax\n"
        "int $0x80\n"
        :
        : "rm"(str), "i"(Syscall::Kprintf)
        : "ebx", "eax"
    );
}

void putc(char c) {
    asm volatile(
        "movl %0, %%ebx\n"
        "movl %1, %%eax\n"
        "int $0x80\n"
        :
        : "rm"(c), "i"(Syscall::Kputc)
        : "ebx", "eax"
    );

}

void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf_internal(putc, fmt, args);
    va_end(args);

}