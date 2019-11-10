#include "stdio.h"
#include "syscalls.h"
#include "stdarg.h"
#include "printf.h"

void puts(const char* str) {
    Syscall::invoke(Syscall::Kprintf, (u32)(void*)str);
}

void putc(char c) {
    Syscall::invoke(Syscall::Kputc, c);
}

void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf_internal(putc, fmt, args);
    va_end(args);

}