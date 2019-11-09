#pragma once

#ifdef KERNEL
void kprint(const char* str);
void kprintf(const char* fmt, ...);
#endif