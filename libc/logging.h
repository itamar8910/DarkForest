#pragma once

#ifndef MODE
#include "mode.h"
#endif

#ifdef KERNEL
void kprint(const char* str);
void kprintf(const char* fmt, ...);
#endif