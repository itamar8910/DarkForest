
#include "assert.h"

void ASSERT(bool x, const char* msg = "[Unnamed]") {
    if(!x) {
        kprintf("\n*** Assertion failed: %s\n", msg);
        cpu_hang();
    }
}