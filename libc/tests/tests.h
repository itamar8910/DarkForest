
#pragma once
#include "logging.h"
#include "tests/test_bits.h"
#include "tests/test_vector.h"
#include "tests/test_list.h"
#include "tests/test_string.h"
#include "tests/test_shared_ptr.h"
#include "tests/test_path.h"

void run_tests() {
    bits_tests();
    vector_tests();
    list_tests();
    string_tests();
    test_shared_ptr();
    path_tests();

    kprintf("*** all tests passed ***\n");
}