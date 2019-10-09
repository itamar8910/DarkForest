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

static_assert(sizeof(u64)==8);

constexpr u32 KB = 1024;
constexpr u32 MB = 0x100000;
constexpr u32 GB = 0x40000000;

typedef u32 Err;