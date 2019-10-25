#pragma once

#include "types.h"
#include "TaskBlocker.h"
#include "types/String.h"

struct TaskMetaData {
    enum class State {
        Running,
        Runnable,
        Blocked,
        Dead,
    };
    String name;
    State state;
    u8 priority;
    TaskBlocker* blocker;
    TaskMetaData(String _name)
        : name(_name),
          state(State::Runnable),
          priority(128),
          blocker(nullptr) {}
};

struct [[gnu::packed]] ThreadControlBlock {
    u32 id;
    void* ESP;
    void* CR3;
    // we have a ptr here to keep the structure simple,
    // because we handle it in ASM
    TaskMetaData* meta_data;
    ThreadControlBlock(String name): id(0), 
                          ESP(nullptr),
                          CR3(nullptr),
                          meta_data(new TaskMetaData(name))
                          {}
    ~ThreadControlBlock() {delete meta_data;}
};

extern ThreadControlBlock* current_TCB;

extern void* next_task_stack_virtual_addr;

extern u32 current_thread_id;

void initialize_multitasking();


void switch_to_task(ThreadControlBlock* next);

ThreadControlBlock* create_kernel_task(void (*func)(), String name="[Unnamed]");

