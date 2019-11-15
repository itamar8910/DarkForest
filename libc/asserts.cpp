#include "asserts.h"


#ifndef MODE
#include "mode.h"
#endif

#ifdef KERNEL
#include "logging.h"
#include "kernel/cpu.h"
#endif
#ifdef USERSPACE
#include "stdio.h"
#include "unistd.h"
#endif

void ASSERT(bool x, const char* msg) {
    if(!x) {
#ifdef KERNEL
        kprintf("\n*** Assertion failed: %s\n", msg);
        cpu_hang();
#endif
#ifdef USERSPACE
    puts("\n** Assertion failed:\n");
    puts(msg);
    exit(1);
#endif
    }
}

void NOT_IMPLEMENTED(const char* msg) {
#ifdef KERNEL
        kprintf("NOT IMPLEMENTED: %s\n", msg);
        cpu_hang();
#endif
#ifdef USERSPACE
    puts("NOT IMPLEMENTED");
    puts(msg);
    exit(1);
#endif
}

[[noreturn]] void ASSERT_NOT_REACHED(const char* msg) {
#ifdef KERNEL
        kprintf("ASSERT NOT REACHED", msg);
        cpu_hang();
#endif
#ifdef USERSPACE
    puts("ASSERT NOT REACHED");
    puts(msg);
    exit(1);
#endif
}

#ifdef KERNEL
void ASSERT_INTERRUPTS_DISABLED() {
    ASSERT(!(cpu_flags() & 0x200), "interrupts not disabled");
}
#endif