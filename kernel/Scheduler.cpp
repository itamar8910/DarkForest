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
    kprintf("current task id: %d\n", m_current_task->id);
    #endif
    
    if(m_current_task == nullptr) {
        ASSERT(m_idle_task != nullptr, "Scheulder::tick() idle task is null");
        m_current_task = m_idle_task;
    }
    if(m_current_task == m_idle_task) {
        // we always want to preempt the idle task
        m_current_task->meta_data->state = TaskMetaData::State::Runnable;
        m_current_task = nullptr;
    }
    else if( 
         m_current_task ->meta_data->state == TaskMetaData::State::Running) {
        // if current task is not blocked
        // check if exceeded time slice
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
    pick_next_and_switch();

}

void Scheduler::pick_next_and_switch() {
    auto* chosen_task = pick_next();
    if(chosen_task != nullptr) {
        #ifdef DBG_SCHEDULER
        print_scheduler_tasks();
        #endif
        bool removed = m_runanble_tasks.remove(chosen_task);
        ASSERT(removed, "Scheduer: failed to remove chosen task from runnable list");
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

void Scheduler::block_current(TaskBlocker* blocker) {
    asm volatile("cli"); // disable interrupts
    m_current_task->meta_data->blocker = blocker;
    m_current_task->meta_data->state = TaskMetaData::State::Blocked;
    m_blocked_tasks.append(m_current_task);

    Scheduler::the().pick_next_and_switch();
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

void Scheduler::terminate() {
    asm volatile("cli");
    kprintf("Task [%s] terminated\n", current_TCB->meta_data->name.c_str());
    ASSERT(current_TCB->id == Scheduler::the().m_current_task->id);
    // this will have the effect of terminating the task because 
    // it will not be added to the runnable list
    delete current_TCB;
    current_TCB = nullptr;
    // TODO: delete userspace resources
    Scheduler::the().m_current_task = nullptr;

    Scheduler::the().pick_next_and_switch();
}


ThreadControlBlock& Scheduler::current_task() {
    return *m_current_task;
}