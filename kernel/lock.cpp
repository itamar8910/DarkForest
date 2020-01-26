#include "lock.h"
#include "logging.h"
#include "kernel/Scheduler.h"
#include "InterruptDisabler.h"

//reference: https://idea.popcount.org/2012-09-12-reinventing-spinlocks/

#define DO_LOCK

// static inline u32 CAS(volatile u32* mem, u32 newval, u32 oldval)
// {
//     u32 ret;
//     asm volatile(
//         "cmpxchgl %2, %1"
//         :"=a"(ret), "+m"(*mem)
//         :"r"(newval), "0"(oldval)
//         :"cc", "memory");
//     return ret;
// }

void Lock::lock()
{
        m_flags = cpu_flags();
    cli();
    // kprintf("lock: %s\n", m_name);
    // #ifdef DO_LOCK
    // while (__sync_bool_compare_and_swap(&m_locked, 0, 1) == 0)
    // {
    //     kprintf("spin\n");
    // }
    // #endif
}

void Lock::unlock()
{
        if (m_flags & 0x200)
            sti();
    // kprintf("unlock: %s\n", m_name);
    // #ifdef DO_LOCK
    // __asm__ __volatile__ ("" ::: "memory");
    // m_locked = 0;
    // #endif
}

Locker::Locker(Lock& lock)
    : m_lock(lock)
{
    m_lock.lock();
}

Locker::~Locker()
{
    m_lock.unlock();
}
