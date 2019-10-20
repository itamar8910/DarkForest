
#include "TaskBlocker.h"
#include "PIT.h"
#include "Kassert.h"


bool SleepBlocker::can_unblock() {
    auto sec = PIT::seconds_since_boot();
    auto ms = PIT::ticks_this_second();
    if(sec < m_sleep_until_sec) {
        return false;
    }
    ASSERT(sec == m_sleep_until_sec, "SleepBlocker: skipped a second?!");
    return ms >= m_leftover_ms;
}