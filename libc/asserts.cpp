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
#include "df_unistd.h"
#endif


void assertion_failed(const char* expression, const char* file, int line,  const char* func)
{
    dbgprintf("\n### Assertion failed: \"%s\" at %s:%d in %s\n",expression, file, line, func);
#ifdef KERNEL
    invoke_crash();
#endif
#ifdef USERSPACE
    std::exit(1);
#endif
    while(1);
}

#ifdef KERNEL
void ASSERT_INTERRUPTS_DISABLED() {
    ASSERT(!(cpu_flags() & 0x200));
}
#endif
