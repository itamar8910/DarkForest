#pragma once

#include "types.h"
#include "flags.h"
#include "logging.h"
#include "Kassert.h"

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
 * returns the index of this first set bit
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

#ifdef TESTS
static void bits_tests() {
    kprintf("[BITS_TESTS]\n");
    u32 x = 5; // 101
    set_bit(x, 3);
    ASSERT(x == 13, "1");
    ASSERT(get_bit(x, 2) == 1, "2");
    ASSERT(get_on_bit_idx(3) == 0, "3");
    ASSERT(get_on_bit_idx(16) == 4, "4");
    ASSERT(get_on_bit_idx(0) == -1, "5");
    x = 13;
    ASSERT(unset_bit(x, 3) == 5, "6");
}
#endif