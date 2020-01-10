
#include "logging.h"
#include "stdarg.h"
#include "printf.h"
#include "types.h"

#ifdef KERNEL
#include "kernel/drivers/DebugPort.h"
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

void print_hexdump(const u8* data, size_t size)
{
    for(size_t i = 0; i < size; i += 16)
    {
        for(size_t j = 0; j < 8; ++j)
        {
            kprintf("0x%x ", data[i+j]);
        }
        kprintf(" ");
        for(size_t j = 8; j < 16; ++j)
        {
            kprintf("0x%x ", data[i+j]);
        }
        kprintf("\n");
    }
}