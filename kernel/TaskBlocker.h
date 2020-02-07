#pragma once

#include "types.h"

class TaskBlocker {

protected:
    TaskBlocker() {}
public:
    virtual bool can_unblock() = 0; // returns whether task can be unblocked
    virtual ~TaskBlocker() {}

};

class SleepBlocker : public TaskBlocker{
public:
    SleepBlocker(u32 sleep_until_sec, u32 leftover_ms)
                : TaskBlocker() ,
                m_sleep_until_sec(sleep_until_sec),
                m_leftover_ms(leftover_ms) {}
    
    virtual bool can_unblock() override;

    virtual ~SleepBlocker() {}

    
private:
    // sleep until (1000 * sec + ms)
    u32 m_sleep_until_sec;
    u32 m_leftover_ms; 

};

class WaitBlocker : public TaskBlocker{
public:
    WaitBlocker();
    virtual bool can_unblock() override;
    virtual ~WaitBlocker() {}
    void waitee_finished() {m_waitee_finished = true;}
    
private:
    size_t m_waitee_finished {false};

};

class PendingMessageBlocker : public TaskBlocker{
public:
    PendingMessageBlocker(u32 pid);
    virtual bool can_unblock() override;
    virtual ~PendingMessageBlocker() override = default;
    
private:
    u32 m_pid;

};