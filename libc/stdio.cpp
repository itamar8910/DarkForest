#include "stdio.h"
#include "syscalls.h"
#include "stdarg.h"
#include "printf.h"
#include "df_unistd.h"
#include "cstring.h"

#ifdef __cplusplus
extern "C" {
#endif

void puts(const char* str) {
    std::write(STDOUT, str, strlen(str));
}

void putc(char c) {
    std::write(STDOUT, &c, 1);
}

void kputc(char c) {
    Syscall::invoke(Syscall::Kputc, c);
}

void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf_internal(putc, fmt, args);
    va_end(args);

}


int getchar() {
    char c;
    int rc = std::read(STDIN, &c, 1);
    if(rc < 0) {
        return rc;
    }
    return c;
}
#ifdef __cplusplus
}
#endif

void kprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf_internal(kputc, fmt, args);
    va_end(args);

}
