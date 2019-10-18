#include "Scheduler.h"
#include "logging.h"

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
    m_tasks.append(tcb);
}

void Scheduler::tick(RegisterDump& regs) {
    (void)regs;
    // kprintf("Scheduler::tick()\n");
    
    if(m_curent_task_idx == -1) {
        m_curent_task_idx = 0;
        switch_to_task(m_tasks.at(0));
        return;
    }

    // cpu_hang();
    if(m_tick_since_switch < 500) {
        m_tick_since_switch++;
        return;
    }
    kprintf("scheduler: switching to task idx: %d\n", m_curent_task_idx);
    // cpu_hang();
    m_curent_task_idx = (m_curent_task_idx+1) % m_tasks.size();
    switch_to_task(m_tasks.at(m_curent_task_idx));
    m_tick_since_switch = 0;
}