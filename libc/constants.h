#pragma once
#include "types.h"
constexpr u32 PAGE_SIZE = 4 * KB;
constexpr size_t SECTOR_SIZE_BYTES = 512;
constexpr size_t SECTOR_SIZE_WORDS = SECTOR_SIZE_BYTES/2;