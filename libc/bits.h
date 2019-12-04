#pragma once

#include "types.h"
#include "flags.h"
#include "logging.h"
#include "asserts.h"

template <typename I>
static inline I set_bit(I& num, u8 bit, bool val) {
    if(val)
        num |= (1<<bit);
    else
        num &= ~(1<<bit);
    return num;
}
template <typename I>
static inline bool get_bit(I num, u8 bit) {
    return (num & (1<<bit)) != 0;
}

/**
 * returns the index of the first set bit
 * (starting for LSb)
 */
static inline int get_on_bit_idx(u32 num) {
    if(!num) // if num has no bits set
        return -1;
    for(int i = 0; i < 32; i++, num>>=1) {
        if(num & 1)
            return i;
    }
    // num has no set bits
    return -1;
}

#define LSB(x) ((x)&0xFF)
#define MSB(x) (((x) >> 8) & 0xFF)
