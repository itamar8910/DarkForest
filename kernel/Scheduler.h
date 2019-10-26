#pragma once

#include "types/list.h"
#include "task.h"
#include "cpu.h"

class Scheduler {

public:
    static Scheduler& the();
    static void initialize(void (*idle_func)());
    
    void tick(RegisterDump& regs);
    void add_task(ThreadControlBlock* tcb);
    void block_current(TaskBlocker* blocker);
    /**
     *  when created a task, stack is arranged
     *   in a way that will call this function
     *  when the task returns from its main function
    */
    static void terminate();

private:
    Scheduler() 
        : m_runanble_tasks(),
          m_blocked_tasks(),
          m_idle_task(nullptr),
          m_current_task(nullptr),
          m_tick_since_switch(0) {}


    void try_unblock_tasks();
    ThreadControlBlock* pick_next();
    void print_scheduler_tasks();
    void pick_next_and_switch();


    List<ThreadControlBlock*> m_runanble_tasks;
    List<ThreadControlBlock*> m_blocked_tasks;
    ThreadControlBlock* m_idle_task;
    ThreadControlBlock* m_current_task;

    u32 m_tick_since_switch;

};