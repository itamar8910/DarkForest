
#include "TaskBlocker.h"
#include "PIT.h"
#include "Kassert.h"


bool SleepBlocker::can_unblock() {
    auto sec = PIT::seconds_since_boot();
    auto ms = PIT::ticks_this_second();
    if(sec < m_sleep_until_sec) {
        return false;
    }
    // e.g it's acceptable that previous tick sec = 0, sleep_until = 0, and now sec = 1, sleep_until = 0
    ASSERT(sec - m_sleep_until_sec <= 1, "SleepBlocker: skipped a second?!");
    return sec > m_sleep_until_sec || ms >= m_leftover_ms;
}