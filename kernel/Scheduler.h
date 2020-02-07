#pragma once

#include "types/list.h"
#include "task.h"
#include "cpu.h"
#include "process.h"

class Scheduler {

public:
    static Scheduler& the();
    static void initialize(void (*idle_func)());
    
    void tick(RegisterDump& regs);
    void add_process(Process*);
    void block_current(TaskBlocker* blocker);
    /**
     *  when created a task, stack is arranged
     *   in a way that will call this function
     *  when the task returns from its main function
    */
    static void terminate_current();
    Process& current();
    Process* get_process(size_t pid);
    Process* get_process_by_name(const String& name);

private:
    Scheduler() 
        : m_runanble_list(),
          m_blocked_list(),
          m_idle_process(nullptr),
          m_current_process(nullptr),
          m_tick_since_switch(0) {}


    void try_unblock_tasks();
    Process* pick_next();
    void print_scheduler_tasks();
    void pick_next_and_switch();


    List<Process*> m_runanble_list;
    List<Process*> m_blocked_list;
    Process* m_idle_process;
    Process* m_current_process;

    u32 m_tick_since_switch;

};