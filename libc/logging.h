#pragma once

#include "types.h"

#ifndef MODE
#include "mode.h"
#endif

#ifdef USERSPACE
#include "stdio.h"
#endif

void print_hexdump(const u8* data, size_t size);

#ifdef KERNEL

void kprint(const char* str);
void kprintf(const char* fmt, ...);
// #define printf(x) kprintf(x);
static const auto& dbgprintf = kprintf;
#endif
