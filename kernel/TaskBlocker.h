#pragma once

#include "types.h"
#include "types/vector.h"
#include "file.h"

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
    ~PendingMessageBlocker() override = default;
    
private:
    u32 m_pid;

};

class PendingInputBlocker : public TaskBlocker{
public:
    PendingInputBlocker(u32 pid, Vector<File*> pending_files);
    bool can_unblock() override;
    ~PendingInputBlocker() override = default;

    enum class Reason : u8
    {
        PendingMessage = 0,
        FdReady,
    };

    u32 ready_fd_idx() const;
    Reason reason() const;
    
private:
    PendingMessageBlocker m_message_blocker;
    Vector<File*> m_pending_files;
    u32 m_ready_fd_idx {0};
    Reason m_reason {Reason::PendingMessage};
};