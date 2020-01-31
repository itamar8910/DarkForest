#pragma once
#include "types.h"
#ifdef KERNEL
#include "cpu.h"

#define cli() asm volatile("cli" :: \
                               : "memory")
#define sti() asm volatile("sti" :: \
                               : "memory")

class InterruptDisabler {
public:
    InterruptDisabler()
    {
        m_flags = cpu_flags();
        cli();
    }

    ~InterruptDisabler()
    {
        if (m_flags & 0x200)
            sti();
    }

private:
    u32 m_flags;
};
/*
class InterruptDisabler
{
public:
    InterruptDisabler()
    {
        asm volatile("cli" ::: "memory");
    }

    ~InterruptDisabler()
    {
        asm volatile("sti" ::: "memory");
    }
};
*/
#endif