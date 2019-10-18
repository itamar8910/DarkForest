#pragma once

#include "vector.h"
#include "task.h"
#include "cpu.h"

class Scheduler {

public:
    static Scheduler& the();
    static void initialize(void (*idle_func)());
    
    void tick(RegisterDump& regs);
    void add_task(ThreadControlBlock* tcb);

private:
    Scheduler() : m_tasks(), m_curent_task_idx(-1), m_tick_since_switch(0) {}
    Vector<ThreadControlBlock*> m_tasks;
    int m_curent_task_idx;
    u32 m_tick_since_switch;

};