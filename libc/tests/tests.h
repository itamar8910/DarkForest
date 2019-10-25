
#pragma once
#include "tests/test_bits.h"
#include "tests/test_vector.h"
#include "tests/test_list.h"
#include "tests/test_string.h"
#include "logging.h"

void run_tests() {
    bits_tests();
    vector_tests();
    list_tests();
    string_tests();

    kprintf("*** all tests passed ***\n");
}