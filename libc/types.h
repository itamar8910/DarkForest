#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef char i8;
typedef unsigned char u8;
typedef short i16;
typedef unsigned short u16;
typedef int i32;
typedef unsigned int u32;
typedef unsigned long long u64;

static const u32 KB = 1024;
static const u32 MB = 0x100000;
static const u32 GB = 0x40000000;

typedef u32 Err;
