#include "lock.h"
#include "logging.h"
#include "kernel/Scheduler.h"
#include "InterruptDisabler.h"
#include "sleep.h"

//reference: https://idea.popcount.org/2012-09-12-reinventing-spinlocks/

#define DO_LOCK


void Lock::lock()
{
    // kprintf("lock: %s\n", m_name);
    #ifdef DO_LOCK
    while (__sync_bool_compare_and_swap(&m_locked, 0, 1) == 0)
    {
        sleep_ms(1);
        // kprintf("spin\n");
    }
    #endif
}

void Lock::unlock()
{
    // kprintf("unlock: %s\n", m_name);
    #ifdef DO_LOCK
    __asm__ __volatile__ ("" ::: "memory");
    m_locked = 0;
    #endif
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

