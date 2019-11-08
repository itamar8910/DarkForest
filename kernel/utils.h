#pragma once
#include "types.h"

static void stack_push(u32** esp, u32 val) {
    *esp -= 1;
    **esp = val;
}