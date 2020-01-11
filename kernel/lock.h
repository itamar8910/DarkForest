#pragma once
#include "types.h"

/**
 * This is a very crappy lock implementation
 * 1. lock()/unlock() are not atomic
 * 2. its a spinlock
 * TODO: fix this
 */

class Lock
{
public:
    Lock(const char* name, int);

    void lock();
    void unlock();

private:
    u32 m_locked {0};
    const char* m_name;
    u32 holder_pid;
};

class Locker
{
public:
    Locker(Lock& lock);
    ~Locker();
private:
    Lock m_lock;
};

// from serenityOS
#define LOCKER(lock) Locker locker(lock)