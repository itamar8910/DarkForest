#pragma once

#include "list.h"
#include "task.h"
#include "cpu.h"

class Scheduler {

public:
    static Scheduler& the();
    static void initialize(void (*idle_func)());
    
    void tick(RegisterDump& regs);
    void add_task(ThreadControlBlock* tcb);
    void sleep_ms(u32 ms);

private:
    Scheduler() 
        : m_runanble_tasks(),
          m_blocked_tasks(),
          m_idle_task(nullptr),
          m_current_task(nullptr),
          m_tick_since_switch(0) {}


    void try_unblock_tasks();
    ThreadControlBlock* pick_next();


    List<ThreadControlBlock*> m_runanble_tasks;
    List<ThreadControlBlock*> m_blocked_tasks;
    ThreadControlBlock* m_idle_task;
    ThreadControlBlock* m_current_task;

    u32 m_tick_since_switch;

};