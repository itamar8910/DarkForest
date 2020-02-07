#include "Scheduler.h"
#include "logging.h"
#include "drivers/PIT.h"
#include "asserts.h"

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
    s_the->m_idle_process = Process::create(idle_func, "idle");
}

void Scheduler::add_process(Process* process) {
    process->task().meta_data->state = TaskMetaData::State::Runnable;
    m_runanble_list.append(process);
}

constexpr u32 TIME_SLICE_MS = 5;

void Scheduler::tick(RegisterDump& regs) {
    (void)regs;
    ASSERT_INTERRUPTS_DISABLED();

    #ifdef DBG_SCHEDULER_2
    kprintf("Scheduler::tick()\n");
    kprintf("current task id: %d\n", m_current_process->pid());
    #endif
    
    if(m_current_process == nullptr) {
        ASSERT(m_idle_process != nullptr);
        m_current_process = m_idle_process;
    }
    if(m_current_process == m_idle_process) {
        // we always want to preempt the idle task
        m_current_process->task().meta_data->state = TaskMetaData::State::Runnable;
        m_current_process = nullptr;
    }
    else if( 
         m_current_process->task().meta_data->state == TaskMetaData::State::Running) {
        // if current task is not blocked
        // check if exceeded time slice
        if(m_tick_since_switch < TIME_SLICE_MS) {
            // continue with current task
            m_tick_since_switch++;
            return;
        } else{
            // preempt task
            m_current_process->task().meta_data->state = TaskMetaData::State::Runnable;
            #ifdef ASSERTS_SCHEDULER
            ASSERT(m_runanble_list.find(m_current_process)==m_runanble_list.end());
            #endif
            m_runanble_list.append(m_current_process);
            m_current_process = nullptr;
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
    auto* chosen_process = pick_next();
    if(chosen_process != nullptr) {
        #ifdef DBG_SCHEDULER
        print_scheduler_tasks();
        #endif
        bool removed = m_runanble_list.remove(chosen_process);
        ASSERT(removed);
    } else {
        chosen_process = m_idle_process;
    }
    m_current_process = chosen_process;
    m_current_process->task().meta_data->state = TaskMetaData::State::Running;
    m_tick_since_switch = 0;
    #ifdef DBG_SCHEDULER
    if(current().name() != "idle")
        kprintf("Scheduler: switched to task: %s\n", current().name().c_str());
    #endif
    switch_to_task(&m_current_process->task());
}

void Scheduler::try_unblock_tasks() {
    for(auto* process_node = m_blocked_list.head(); process_node != nullptr; process_node = process_node->next) {
        auto* process = process_node->val;

        ASSERT(process->task().meta_data->state == TaskMetaData::State::Blocked);
        TaskBlocker* blocker = process->task().meta_data->blocker;
        ASSERT(blocker != nullptr);
        if(blocker->can_unblock()) {
            #ifdef DBG_SCHEDULER
            kprintf("Scheduler: unblocking: %d\n", process->pid());
            #endif
             process->task().meta_data->state = TaskMetaData::State::Runnable;
            delete process->task().meta_data->blocker;
            process->task().meta_data->blocker = nullptr;
            m_blocked_list.remove(process);
            m_runanble_list.append(process);
        }
    }
}

Process* Scheduler::pick_next() {
    if(m_runanble_list.size() == 0) {
        return nullptr;
    }
    ASSERT(m_runanble_list.head() != nullptr);
    auto* process = m_runanble_list.head()->val;
    ASSERT(process->task().meta_data->state == TaskMetaData::State::Runnable);
    return process;
}

void Scheduler::block_current(TaskBlocker* blocker) {
    asm volatile("cli"); // disable interrupts
    m_current_process->task().meta_data->blocker = blocker;
    m_current_process->task().meta_data->state = TaskMetaData::State::Blocked;
    m_blocked_list.append(m_current_process);

    Scheduler::the().pick_next_and_switch();
}

void Scheduler::print_scheduler_tasks() {
    kprintf("runnable tasks:\n");
    for(auto* process_node = m_runanble_list.head(); process_node != nullptr; process_node = process_node->next) {
        kprintf("%s,", process_node->val->name().c_str());
    }
    kprintf("\nblocked processs:\n");
    for(auto* process_node = m_blocked_list.head(); process_node != nullptr; process_node = process_node->next) {
        kprintf("%d,", process_node->val->name());
    }
    kprintf("\n");
}

void Scheduler::terminate_current() {
    asm volatile("cli");
    kprintf("Process [%s] terminated\n", Scheduler::the().m_current_process->name().c_str());
    ASSERT(current_TCB->id == Scheduler::the().m_current_process->task().id);
    // this will have the effect of terminating the task because 
    // it will not be added to the runnable list
    // delete current_TCB;
    delete Scheduler::the().m_current_process;
    current_TCB = nullptr;
    // TODO: free userspace resources
    Scheduler::the().m_current_process = nullptr;

    Scheduler::the().pick_next_and_switch();
}


Process& Scheduler::current() {
    return *m_current_process;
}

Process* Scheduler::get_process(size_t pid)
{
    auto i1 = m_runanble_list.find([&](Process* other){return other->pid()==pid;});
    if(i1 != m_runanble_list.end()) {
        return *i1;
    }
    auto i2 = m_blocked_list.find([&](Process* other){return other->pid()==pid;});
    if(i2 != m_blocked_list.end()) {
        return *i2;
    }
    return nullptr;
}

Process* Scheduler::get_process_by_name(const String& name)
{
    auto i1 = m_runanble_list.find([&](Process* other){return other->name() == name;});
    if(i1 != m_runanble_list.end()) {
        return *i1;
    }
    auto i2 = m_blocked_list.find([&](Process* other){return other->name() == name;});
    if(i2 != m_blocked_list.end()) {
        return *i2;
    }
    return nullptr;
}