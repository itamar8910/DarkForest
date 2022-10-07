
#pragma once

#include "types.h"
#include "stdarg.h"

int snprintf(char *str, size_t size, const char *format, ...);
int vsnprintf_impl(char * str, size_t size, const char * fmt, va_list args);