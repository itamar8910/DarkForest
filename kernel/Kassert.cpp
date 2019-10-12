
#include "Kassert.h"

void ASSERT(bool x, const char* msg = "[Unnamed]") {
    if(!x) {
        kprintf("\n*** Assertion failed: %s\n", msg);
        cpu_hang();
    }
}

void NOT_IMPLEMENTED(const char* msg = "[Unnamed]") {
    kprintf("\n*** NOT IMPLEMENTED: %s\n", msg);
    cpu_hang();
}

void ASSERT_NOT_REACHED(const char* msg) {
    kprintf("\n*** ASSERT NOT REACHED: %s\n", msg);
    cpu_hang();
}