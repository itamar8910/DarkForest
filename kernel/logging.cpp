
#include "logging.h"
#include "DebugPort.h"
#include "stdarg.h"

#include "printf.h"
// #include "cross-stdarg.h"

void kprint(const char* str) {
    DebugPort::write(str);
}


void kprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf_internal(DebugPort::putc, fmt, args);
    va_end(args);

}