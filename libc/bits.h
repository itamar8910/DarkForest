#pragma once

#include "types.h"

static inline void set_bit(u32& byte, u8 bit) {
    byte |= (1<<bit);
}
static inline bool get_bit(u32 byte, u8 bit) {
    return (byte & (1<<bit)) != 0;
}