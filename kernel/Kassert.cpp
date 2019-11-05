
#include "Kassert.h"

void ASSERT(bool x, const char* msg) {
    if(!x) {
        kprintf("\n*** Assertion failed: %s\n", msg);
        cpu_hang();
    }
}

void NOT_IMPLEMENTED(const char* msg) {
    kprintf("\n*** NOT IMPLEMENTED: %s\n", msg);
    cpu_hang();
}

[[noreturn]] void ASSERT_NOT_REACHED(const char* msg) {
    kprintf("\n*** ASSERT NOT REACHED: %s\n", msg);
    cpu_hang();
}

void ASSERT_INTERRUPTS_DISABLED() {
    ASSERT(!(cpu_flags() & 0x200), "interrupts not disabled");
}