
#include "logging.h"
#include "asserts.h"
#include "bits.h"
#include "cstring.h"

static void bits_tests() {
    kprintf("[BITS_TESTS]\n");
    u32 x = 5; // 101
    set_bit(x, 3, true);
    ASSERT(x == 13);
    ASSERT(get_bit(x,3));
    ASSERT(get_on_bit_idx(3) == 0);
    ASSERT(get_on_bit_idx(16) == 4);
    ASSERT(get_on_bit_idx(0) == -1);
    x = 13;
    ASSERT(set_bit(x,3,true));
    char a1[] = {1,2,3, 0};
    char a2[] = {1,2,4, 0};
    ASSERT(memcmp(a1, a2, 3) != 0);
}
