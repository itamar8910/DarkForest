
#include "Kassert.h"

void ASSERT(bool x, const char* msg = "[Unnamed]") {
    if(!x) {
        kprintf("\n*** Assertion failed: %s\n", msg);
        cpu_hang();
    }
}

void NOT_IMPLEMENTED(const char* msg = "[Unnamed]") {
    kprintf("NOT IMPLEMENTED: %s\n", msg);
    cpu_hang();
}