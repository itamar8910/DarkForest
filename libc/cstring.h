#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t strlen(const char* str);
void* memset(void* s, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void* strcpy(char* dest, const char* src);
void* strncpy(char* dest, const char* src, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strdup(const char *s);
char *strstr(const char *haystack, const char *needle);


#ifdef __cplusplus
}
#endif
void ascii_to_unicode(char* dst, const char* src, size_t num_chars);
