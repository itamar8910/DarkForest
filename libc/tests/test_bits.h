
#include "logging.h"
#include "Kassert.h"
#include "bits.h"

static void bits_tests() {
    kprintf("[BITS_TESTS]\n");
    u32 x = 5; // 101
    set_bit(x, 3, true);
    ASSERT(x == 13, "1");
    ASSERT(get_bit(x, 2) == 1, "2");
    ASSERT(get_on_bit_idx(3) == 0, "3");
    ASSERT(get_on_bit_idx(16) == 4, "4");
    ASSERT(get_on_bit_idx(0) == -1, "5");
    x = 13;
    ASSERT(set_bit(x, 3, false) == 5, "6");
}