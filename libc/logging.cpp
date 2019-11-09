
#include "logging.h"
#include "stdarg.h"
#include "printf.h"

#ifdef KERNEL
#include "drivers/DebugPort.h"
#endif
#ifdef USERSPACE
#include "stdio.h"
#endif


#ifdef KERNEL
void kprint(const char* str) {
    DebugPort::write(str);
}


void kprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf_internal(DebugPort::putc, fmt, args);
    va_end(args);

}
#endif