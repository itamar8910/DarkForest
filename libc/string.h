#pragma once

#include <stdint.h>
#include <stddef.h>

size_t strlen(const char* str);
void* memset(void* s, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t n);
void* strcpy(char* dest, const char* src);
void* strncpy(char* dest, const char* src, size_t n);