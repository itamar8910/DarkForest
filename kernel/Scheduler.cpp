#include "Scheduler.h"
#include "logging.h"
#include "PIT.h"

// #define DBG_SCHEDULER

static Scheduler* s_the = nullptr;

Scheduler& Scheduler::the() {
    return *s_the;
}

void Scheduler::initialize(void (*idle_func)()) {
    s_the = new Scheduler();
    initialize_multitasking();
    auto* idle_tcb = create_kernel_task(idle_func);
    the().add_task(idle_tcb);
}

void Scheduler::add_task(ThreadControlBlock* tcb) {
    kprintf("adding kernel task: 0x%x, id: %d\n", tcb, tcb->id);
    m_tasks.append(tcb);
}

constexpr u32 TIME_SLICE_MS = 5;

void Scheduler::tick(RegisterDump& regs) {
    (void)regs;

    #ifdef DBG_SCHEDULER
    kprintf("Scheduler::tick()\n");
    #endif
    
    if(m_curent_task_idx == -1) {
        m_curent_task_idx = 0;
        m_tasks.at(0)->meta_data->state = TaskMetaData::State::Running;
        switch_to_task(m_tasks.at(0));
        return;
    }

    // if current task is not blocked
    // check if exceeded time slice
    ThreadControlBlock* current_task = m_tasks.at(m_curent_task_idx);
    #ifdef DBG_SCHEDULER
    kprintf("current task id: %d\n", current_task->id);
    #endif
    if(current_task
        ->meta_data->state == TaskMetaData::State::Running) {
        if(m_tick_since_switch < TIME_SLICE_MS) {
            // continue with current task
            m_tick_since_switch++;
            return;
        } else{
            // preempt task
            current_task->meta_data->state = TaskMetaData::State::Runnable;
        }
    }
    try_unblock_tasks();

    m_curent_task_idx = pick_next();
    ThreadControlBlock* next_task = m_tasks.at(m_curent_task_idx);
    next_task->meta_data->state = TaskMetaData::State::Running;
    // switch_to_task(m_tasks.at(m_curent_task_idx));
    switch_to_task(next_task);
    m_tick_since_switch = 0;
}

void Scheduler::try_unblock_tasks() {
    for(size_t i = 0; i < m_tasks.size(); i++) {
        ThreadControlBlock* task = m_tasks.at(i);

        if(task->meta_data->state != TaskMetaData::State::Blocking)
            continue;
        TaskBlocker* blocker = task->meta_data->blocker;
        ASSERT(blocker != nullptr, "Task is blocked but has no TaskBlocker");
        if(blocker->can_unblock()) {
            #ifdef DBG_SCHEDULER
            kprintf("Scheduler: unblocking: %d\n", task->id);
            #endif
            task->meta_data->state = TaskMetaData::State::Runnable;
            delete task->meta_data->blocker;
            task->meta_data->blocker = nullptr;
        }
    }
}

size_t Scheduler::pick_next() {
    // TODO: switch to a list
    size_t c = 0;
    for(size_t i = (m_curent_task_idx+1)%m_tasks.size(); c < m_tasks.size(); i=(i+1)%m_tasks.size(), c++) {
        ThreadControlBlock* task = m_tasks.at(i);
        ASSERT(task->meta_data->state != TaskMetaData::State::Running, 
            "Scheduler::pick_next: there cannot be a running task here");
        if(task->meta_data->state != TaskMetaData::State::Runnable)
            continue;
        return i;
    }
    // TODO: have idle only run in this case
    ASSERT_NOT_REACHED("Scheduler::pick_next could not find a runnable task");
    return 0;
}

void Scheduler::sleep_ms(u32 ms) {
    ThreadControlBlock& current = *m_tasks[m_curent_task_idx];
    #ifdef DBG_SCHEDULER
    kprintf("task: %d - sleep_ms: %d\n", current.id, ms);
    #endif
    u32 sleep_until_sec = PIT::seconds_since_boot() + (ms / 1000);
    u32 leftover_ms = PIT::ticks_this_second() + (ms % 1000);
    if(leftover_ms > 1000) {
        sleep_until_sec += 1;
        leftover_ms %= 1000;
    }
    SleepBlocker* blocker = new SleepBlocker(sleep_until_sec, leftover_ms);
    current.meta_data->blocker = blocker;
    current.meta_data->state = TaskMetaData::State::Blocking;
    asm volatile("hlt"); // stop until next interrupt
}