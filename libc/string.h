#pragma once

#include <stdint.h>
#include <stddef.h>

size_t strlen(const char* str);
void* memset(void* s, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t n);