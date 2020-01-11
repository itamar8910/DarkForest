#include "lock.h"
#include "logging.h"
#include "kernel/Scheduler.h"

#define DO_LOCK

Lock::Lock(const char* name, int)
    : m_name(name)
{

}

static inline u32 CAS(volatile u32* mem, u32 newval, u32 oldval)
{
    u32 ret;
    asm volatile(
        "cmpxchgl %2, %1"
        :"=a"(ret), "+m"(*mem)
        :"r"(newval), "0"(oldval)
        :"cc", "memory");
    return ret;
}

void Lock::lock()
{
    #ifdef DO_LOCK
    kprintf("waiting for %s lock\n", m_name);
    while(CAS(&m_locked, 1, 0) != 0) {}
    __asm__ ("mfence");
    kprintf("acuired lock\n");
    #endif
}

void Lock::unlock()
{
    #ifdef DO_LOCK
    kprintf("released %s lock\n", m_name);
    __asm__ ("mfence");
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
