#pragma once
#include "types.h"

#define CODE_ASSERT_NOT_REACHED 127

[[noreturn]] void exit(int status);

void sleep_ms(u32 ms);

int open(const char* path);
int ioctl(int fd, u32 code, void* data);