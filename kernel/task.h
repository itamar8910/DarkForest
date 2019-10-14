#pragma once

#include "types.h"

struct [[gnu::packed]] ThreadControlBlock {
    u32 id;
    void* ESP;
    void* CR3;
};

extern ThreadControlBlock* current_TCB;

extern void* next_task_stack_virtual_addr;

extern u32 current_thread_id;

void initialize_multitasking();


void switch_to_task(ThreadControlBlock* next);

ThreadControlBlock* create_kernel_task(void (*func)());