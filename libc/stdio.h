#pragma once

#include "types.h"

static const size_t STDIN = 0;
static const size_t STDOUT = 1;
static const size_t STDERR = 2;
static const size_t stdin = STDIN;
static const size_t stdout = STDOUT;
static const size_t stderr = STDERR;

void puts(const char* str);
void putc(char);
void kputc(char c);
void printf(const char* fmt, ...);
void kprintf(const char* fmt, ...);
int getchar();

#define dbgprintf kprintf

// static const auto& dbgprintf = kprintf;
