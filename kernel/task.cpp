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

ThreadControlBlock* create_kernel_task(void (*func)()) {
    ThreadControlBlock* tcb = new ThreadControlBlock();
    tcb->id = ++current_thread_id;
    tcb->CR3 = (void*) get_cr3();
    // allocate stack space
    MemoryManager::the().allocate((u32)next_task_stack_virtual_addr, true, false);
    u32* new_stack = (u32*)((u32)next_task_stack_virtual_addr + PAGE_SIZE - 4);
    // increment stack space for next task to be created bya page
    next_task_stack_virtual_addr = (void*)((u32)next_task_stack_virtual_addr + PAGE_SIZE);
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

    tcb->ESP = new_stack;

    return tcb;
}