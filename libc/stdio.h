#pragma once

#include "types.h"
#include "stdarg.h"

static const size_t STDIN = 0;
static const size_t STDOUT = 1;
static const size_t STDERR = 2;

typedef size_t FILE;

static const size_t stdin = STDIN;
static const size_t stdout = STDOUT;
static const size_t stderr = STDERR;


#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#ifdef __cplusplus
extern "C" {
#endif

void puts(const char* str);
void putc(char);
int putchar(int);
void kputc(char c);
void printf(const char* fmt, ...);
int getchar();

int fclose(FILE *stream);
FILE *fopen(const char *pathname, const char *mode);
int fprintf(FILE *stream, const char *format, ...);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
int fflush(FILE *stream);
int64_t __divdi3(int64_t a, int64_t b);
uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint64_t* rem_p);
int remove(const char *pathname);
int rename(const char *oldpath, const char *newpath);
int snprintf(char *str, size_t size, const char *format, ...);
int sscanf(const char *str, const char *format, ...);
int vprintf(const char *format, va_list ap);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
int vfprintf(FILE *stream, const char *format, va_list ap);


#ifdef __cplusplus
}
#endif

void kprintf(const char* fmt, ...);

#define dbgprintf kprintf

// static const auto& dbgprintf = kprintf;
