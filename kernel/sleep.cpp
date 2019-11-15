
#include "sleep.h"
#include "Scheduler.h"
#include "drivers/PIT.h"

void sleep_ms(u32 ms) {
    #ifdef DBG_SCHEDULER
    kprintf("task: %d - sleep_ms: %d\n", m_current_task->id, ms);
    #endif
    asm volatile("cli");
    u32 sleep_until_sec = PIT::seconds_since_boot() + (ms / 1000);
    u32 leftover_ms = PIT::ticks_this_second() + (ms % 1000);
    if(leftover_ms > 1000) {
        sleep_until_sec += 1;
        leftover_ms %= 1000;
    }
    SleepBlocker* blocker = new SleepBlocker(sleep_until_sec, leftover_ms);
    Scheduler::the().block_current(blocker);
}