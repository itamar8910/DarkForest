#include "task.h"
#include "cpu.h"
#include "MM/MemoryManager.h"
#include "Scheduler.h"
#include "utils.h"

extern "C" void asm_switch_to_task(ThreadControlBlock* next);

// #define DBG_TASKSWITCH

ThreadControlBlock* current_TCB;

void* next_task_stack_virtual_addr = (void*)0xd0000000;

u32 current_thread_id = 1;

void initialize_multitasking() {
    // just a dummy initial TCB
    current_TCB = new ThreadControlBlock("dummy");
    current_TCB->id = 1;
    current_TCB->CR3 = (void*) get_cr3();
    current_TCB->ESP = 0;
}

void switch_to_task(ThreadControlBlock* next) {
    #ifdef DBG_TASKSWITCH
    kprintf("Switching to Task: %d (%s)\n", next->id, next->meta_data->name.c_str());
    #endif
    asm_switch_to_task(next);
}

#define CLONE_PAGE_DIRECTORY

ThreadControlBlock* create_kernel_task(void (*func)(), String name) {
    ThreadControlBlock* tcb = new ThreadControlBlock(name);
    tcb->id = ++current_thread_id;
    // allocate stack space for new task
    kprintf("new CR3: 0x%x\n", (u32)tcb->CR3);
    MemoryManager::the().allocate((u32)next_task_stack_virtual_addr,
        PageWritable::YES,
        UserAllowed::NO);
    u32* new_stack = (u32*)((u32)next_task_stack_virtual_addr + PAGE_SIZE - 4);
    // initialize value of stack (will be popped off at the end of switch_to_task)
    stack_push(&new_stack, u32(Scheduler::terminate)); // jumps here after func returns
    stack_push(&new_stack, (u32)func); // will be popped of as EIP
    stack_push(&new_stack, 0); // ebp
    stack_push(&new_stack, 0); // edi
    stack_push(&new_stack, 0); // esi
    stack_push(&new_stack, 0); // ebx

    kprintf("new task stack: stack: 0x%x, content: 0x%x,0x%x,0x%x,0x%x,0x%x\n",
            new_stack,
            new_stack[0],
            new_stack[1],
            new_stack[2],
            new_stack[3],
            new_stack[4]
            );

    #ifdef CLONE_PAGE_DIRECTORY
    tcb->CR3 = (void*) (u32)MemoryManager::the().clone_page_directory().get_base();
    #else
    tcb->CR3 = (void*) (u32)get_cr3();
    #endif
    next_task_stack_virtual_addr = (void*)((u32)next_task_stack_virtual_addr + PAGE_SIZE);
    tcb->ESP = new_stack;
    tcb->ESP0 = new_stack;

    return tcb;
}