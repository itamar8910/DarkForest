#pragma once

#include "cpu.h"
#include "logging.h"

void ASSERT(bool x, const char* msg = "[Unnamed]") {
    if(!x) {
        kprintf("Assertion failed: %s\n", msg);
        cpu_hang();
    }
}