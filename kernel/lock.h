#pragma once
#include "types.h"
#include "NonCopyable.h"

class Lock
{
NON_COPYABLE(Lock)
public:
    Lock(const char* name) : m_name(name){}
    void lock();
    void unlock();

private:
    u32 m_locked {0};
    const char* m_name = nullptr;
};

class Locker
{
NON_COPYABLE(Locker)
public:
    Locker(Lock& lock);
    ~Locker();

private:
    Lock& m_lock;
};

#define LOCKER(lock) Locker locker(lock)