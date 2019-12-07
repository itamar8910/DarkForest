#pragma once

#ifndef MODE
#include "mode.h"
#endif

#ifdef USERSPACE
#include "stdio.h"
#endif

#ifdef KERNEL
void kprint(const char* str);
void kprintf(const char* fmt, ...);
// #define printf(x) kprintf(x);
static const auto& dbgprintf = kprintf;
#endif