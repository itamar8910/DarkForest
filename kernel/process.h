#pragma once
#include "task.h"

class Process {
public:
    static Process* create(void (*main)(), String name="[Unnamed]");

    Process(u32 pid, ThreadControlBlock* task, String name)
        : m_pid(pid),
          m_task(task),
          m_name(name) {}

    ThreadControlBlock& task() {return *m_task;}
    u32 pid(){return m_pid;}
    String name(){return m_name;}

    ~Process();

private:
    u32 m_pid;
    ThreadControlBlock* m_task;
    String m_name;
};