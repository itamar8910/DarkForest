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


extern "C" {
    
#ifdef USERSPACE
static void print_backtrace(){
    dbgprintf("BACKTRACE:\n");
    uint32_t** ebp = 0;
    uint32_t* eip = 0;
    __asm__("movl %%ebp, %[fp]" :  /* output */ [fp] "=r" (ebp));

    while (ebp && (uint32_t)ebp < 0xc0000000) {
        eip = ebp[1];
        dbgprintf("%p (llvm-addr2line-10 -e doomgeneric %p)\n", eip, eip);
        if (!eip)
            break;
        ebp = (uint32_t**)ebp[0];
    }

}
#endif


void assertion_failed(const char* expression, const char* file, int line,  const char* func)
{
#ifdef USERSPACE
    printf("\n### Assertion failed: \"%s\" at %s:%d in %s\n",expression, file, line, func);
#endif
    dbgprintf("\n### Assertion failed: \"%s\" at %s:%d in %s\n",expression, file, line, func);
#ifdef KERNEL
    invoke_crash();
#endif
#ifdef USERSPACE
    print_backtrace();
    std::exit(1);
#endif
    while(1);
}
}

#ifdef KERNEL
void ASSERT_INTERRUPTS_DISABLED() {
    ASSERT(!(cpu_flags() & 0x200));
}
#endif
