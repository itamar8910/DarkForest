
#include "TaskBlocker.h"
#include "drivers/PIT.h"
#include "asserts.h"
#include "Scheduler.h"


bool SleepBlocker::can_unblock() {
    auto sec = PIT::seconds_since_boot();
    auto ms = PIT::ticks_this_second();
    if(sec < m_sleep_until_sec) {
        return false;
    }
    // we can skeep at most a single second
    
    // we can skip a single second if
    // SleepBlocker::can_unblock()
    // was called once at (e.g) 1.999sec,
    // and then at 2.001sec
    
    ASSERT(sec - m_sleep_until_sec <= 1);
    return sec > m_sleep_until_sec || ms >= m_leftover_ms;
}

WaitBlocker::WaitBlocker()
    : TaskBlocker()
    {}

bool WaitBlocker::can_unblock()
{
    return m_waitee_finished;
}

PendingMessageBlocker::PendingMessageBlocker(u32 pid)
    : m_pid(pid)
{
}


bool PendingMessageBlocker::can_unblock()
{
    Process* p = Scheduler::the().get_process(m_pid);
    ASSERT(p != nullptr);
    return p->has_pending_message();
}

PendingInputBlocker::PendingInputBlocker(u32 pid, Vector<File*> pending_files, u32& ready_fd_idx, Reason& reason) :
    m_message_blocker(pid),
    m_pending_files(pending_files),
    m_out_ready_fd_idx(ready_fd_idx),
    m_out_reason(reason)
    {}

bool PendingInputBlocker::can_unblock()
{
    if(m_message_blocker.can_unblock())
    {
        m_out_reason = Reason::PendingMessage;
        return true;
    }


    for(u32 fd_idx = 0; fd_idx < m_pending_files.size(); ++fd_idx)
    {
        if(m_pending_files[fd_idx]->can_read())
        {
            m_out_ready_fd_idx = fd_idx;
            m_out_reason = Reason::FdReady;
            return true;
        }
    }
    return false;
}
