#include "process.h"
#include "task.h"

u32 g_next_pid;

Process* Process::create(void (*main)(), String name) {
    auto* task = create_kernel_task(main);
    return new Process(g_next_pid++, task, name);
}


Process::~Process() {
    // TODO: free usespace resources & pages
    delete m_task;
}