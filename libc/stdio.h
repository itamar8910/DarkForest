#pragma once

#include "types.h"

constexpr size_t STDIN = 0;
constexpr size_t STDOUT = 1;
constexpr size_t STDERR = 2;

void puts(const char* str);
void putc(char);
void kputc(char c);
void printf(const char* fmt, ...);
void kprintf(const char* fmt, ...);
int getchar();

static const auto& dbgprintf = kprintf;