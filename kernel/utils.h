#pragma once
#include "types.h"

#define GENERATE_SEGFAULT


void  __attribute__ ((noinline)) test_generate_segfault() {
    char* p = nullptr;
    *p = 0;
}
void  __attribute__ ((noinline)) test_generate_segfault_wrapper() {
    test_generate_segfault();
}

static void stack_push(u32** esp, u32 val) {
    #ifdef GENERATE_SEGFAULT
    test_generate_segfault_wrapper();
    #endif
    *esp -= 1;
    **esp = val;
}