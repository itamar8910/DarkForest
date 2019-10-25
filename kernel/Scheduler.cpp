#include "Scheduler.h"
#include "logging.h"
#include "PIT.h"
#include "Kassert.h"

// #define DBG_SCHEDULER
// #define DBG_SCHEDULER_2

#define ASSERTS_SCHEDULER

static Scheduler* s_the = nullptr;

Scheduler& Scheduler::the() {
    return *s_the;
}

void Scheduler::initialize(void (*idle_func)()) {
    s_the = new Scheduler();
    initialize_multitasking();
    s_the->m_idle_task = create_kernel_task(idle_func, "idle");
}

void Scheduler::add_task(ThreadControlBlock* tcb) {
    kprintf("adding kernel task: 0x%x, id: %d\n", tcb, tcb->id);
    tcb->meta_data->state = TaskMetaData::State::Runnable;
    m_runanble_tasks.append(tcb);
}

constexpr u32 TIME_SLICE_MS = 5;

void Scheduler::tick(RegisterDump& regs) {
    (void)regs;
    ASSERT_INTERRUPTS_DISABLED();

    #ifdef DBG_SCHEDULER_2
    kprintf("Scheduler::tick()\n");
    #endif
    
    if(m_current_task == nullptr) {
        kprintf("Shceduller:: first tick()\n");
        // m_curent_task_idx = 0;
        // m_tasks.at(0)->meta_data->state = TaskMetaData::State::Running;
        // switch_to_task(m_tasks.at(0));
        ASSERT(m_idle_task != nullptr, "Scheulder::tick() idle task is null");
        m_current_task = m_idle_task;
        switch_to_task(m_idle_task);
        return;
    }

    // if current task is not blocked
    // check if exceeded time slice
    #ifdef DBG_SCHEDULER_2
    kprintf("current task id: %d\n", m_current_task->id);
    #endif
    if(m_current_task == m_idle_task) {
        // we always want to preempt the idle task
        m_current_task->meta_data->state = TaskMetaData::State::Runnable;
        m_current_task = nullptr;
    }
    else if( 
         m_current_task ->meta_data->state == TaskMetaData::State::Running) {
        if(m_tick_since_switch < TIME_SLICE_MS) {
            // continue with current task
            m_tick_since_switch++;
            return;
        } else{
            // preempt task
            m_current_task->meta_data->state = TaskMetaData::State::Runnable;
            #ifdef ASSERTS_SCHEDULER
            ASSERT(!m_runanble_tasks.find(m_current_task), "currently runing task should not be in runnable list");
            #endif
            m_runanble_tasks.append(m_current_task);
            m_current_task = nullptr;
        }
    }
    // NOTE: in the current implementation,
    // we only try to unblock tasks when the current task is preemtped / blocked
    // this may be problematic if a higher priority task is blocked
    // - we will only check if it can be unblocekd once the current task time slice is done (or if current task has been blocked)


    // if we got here, current task has finished its time slice / is blocked

    try_unblock_tasks();

    auto* chosen_task = pick_next();
    if(chosen_task != nullptr) {
        #ifdef DBG_SCHEDULER
        print_scheduler_tasks();
        #endif
        ASSERT(m_runanble_tasks.remove(chosen_task), "Scheduer: failed to remove chosen task from runnable list");
    } else {
        chosen_task = m_idle_task;
    }
    m_current_task = chosen_task;
    m_current_task->meta_data->state = TaskMetaData::State::Running;
    m_tick_since_switch = 0;
    switch_to_task(m_current_task);
}

void Scheduler::try_unblock_tasks() {
    for(auto* task_node = m_blocked_tasks.head(); task_node != nullptr; task_node = task_node->next) {
        auto* task = task_node->val;

        ASSERT(task->meta_data->state == TaskMetaData::State::Blocked, "Scheduler: task is in blocked list but not blocked");
        TaskBlocker* blocker = task->meta_data->blocker;
        ASSERT(blocker != nullptr, "Task is blocked but has no TaskBlocker");
        if(blocker->can_unblock()) {
            #ifdef DBG_SCHEDULER
            kprintf("Scheduler: unblocking: %d\n", task->id);
            #endif
            task->meta_data->state = TaskMetaData::State::Runnable;
            delete task->meta_data->blocker;
            task->meta_data->blocker = nullptr;
            m_blocked_tasks.remove(task);
            m_runanble_tasks.append(task);
        }
    }
}

ThreadControlBlock* Scheduler::pick_next() {
    if(m_runanble_tasks.size() == 0) {
        return nullptr;
    }
    ASSERT(m_runanble_tasks.head() != nullptr, "Scheduler: runnable_tasks head is null but size != 0");
    auto* task = m_runanble_tasks.head()->val;
    ASSERT(task->meta_data->state == TaskMetaData::State::Runnable, "Scheduler: task is in runnable list but not runnable");
    return task;
}

void Scheduler::sleep_ms(u32 ms) {
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
    m_current_task->meta_data->blocker = blocker;
    m_current_task->meta_data->state = TaskMetaData::State::Blocked;
    m_blocked_tasks.append(m_current_task);
    asm volatile("sti");
    asm volatile("hlt"); // stop until next interrupt
}

void Scheduler::print_scheduler_tasks() {
    kprintf("runnable tasks:\n");
    for(auto* task_node = m_runanble_tasks.head(); task_node != nullptr; task_node = task_node->next) {
        kprintf("%d,", task_node->val->id);
    }
    kprintf("\nblocked tasks:\n");
    for(auto* task_node = m_blocked_tasks.head(); task_node != nullptr; task_node = task_node->next) {
        kprintf("%d,", task_node->val->id);
    }
    kprintf("\n");
}