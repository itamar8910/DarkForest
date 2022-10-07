#include "stdio_shared.h"
#include "printf.h"
#include "stdio.h"

int snprintf(char * str, size_t n, const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int rc = vsnprintf_impl(str, n, fmt, args);
    va_end(args);
    return rc;
}

int vsnprintf_impl(char * str, size_t size, const char * fmt, va_list args)
{
    size_t char_index = 0;
    auto putc_func = [&](char c) {
        if (char_index < size) {
            str[char_index] = c;
        }
        ++char_index;
    };

    printf_internal(putc_func, fmt, args);

    if (char_index < size) {
        str[char_index] = '\0';
    } else if (size > 0) {
        str[size - 1] = '\0';
    }

    return char_index;
}