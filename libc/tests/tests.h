
#pragma once
#include "tests/test_bits.h"
#include "tests/test_vector.h"
#include "logging.h"

void run_tests() {
    bits_tests();
    vector_tests();

    kprintf("*** all tests passed ***\n");
}