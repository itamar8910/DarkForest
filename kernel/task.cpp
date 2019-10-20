#include "task.h"
#include "cpu.h"
#include "MemoryManager.h"

extern "C" void asm_switch_to_task(ThreadControlBlock* next);

ThreadControlBlock* current_TCB;

void* next_task_stack_virtual_addr = (void*)0xd0000000;

u32 current_thread_id = 1;

void initialize_multitasking() {
    // just a dummy initial TCB
    current_TCB = new ThreadControlBlock();
    current_TCB->id = 1;
    current_TCB->CR3 = (void*) get_cr3();
    current_TCB->ESP = 0;
}

void switch_to_task(ThreadControlBlock* next) {
    kprintf("switch_to_task: ID %d\n", next->id);
    asm_switch_to_task(next);
    kprintf("done\n");
}

void stack_push(u32** esp, u32 val) {
    *esp -= 1;
    **esp = val;
}

/**
 * TODO: if we want to CLONE_PAGE_DIRECTORY when create a new kernel task
 * we need to modify MM::clone_page_directory
 * to only clone page tables in USER SPACE
 * otherwise, when one kernel task changes its memory map (e.g KMalloc allocating more pages)
 * the change does not affect other kernel tasks because they have different page tables
 * which leads to problems (page faults when accessing shared data between tasks, data discrepancies etc)
 * 
*/
// #define CLONE_PAGE_DIRECTORY

ThreadControlBlock* create_kernel_task(void (*func)()) {
    ThreadControlBlock* tcb = new ThreadControlBlock();
    tcb->id = ++current_thread_id;
    // allocate stack space for new task
    kprintf("new CR3: 0x%x\n", (u32)tcb->CR3);
    MemoryManager::the().allocate((u32)next_task_stack_virtual_addr, true, false);
    u32* new_stack = (u32*)((u32)next_task_stack_virtual_addr + PAGE_SIZE - 4);
    // initialize value of stack (will be popped off at the end of switch_to_task)
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
    MemoryManager::the().deallocate((u32)next_task_stack_virtual_addr, false);
    #else
    tcb->CR3 = (void*) (u32)get_cr3();
    #endif
    // deallocate of the new task
    next_task_stack_virtual_addr = (void*)((u32)next_task_stack_virtual_addr + PAGE_SIZE);
    tcb->ESP = new_stack;

    return tcb;
}