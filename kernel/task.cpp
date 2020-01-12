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
    current_TCB = new ThreadControlBlock();
    current_TCB->id = 1;
    current_TCB->CR3 = (void*) get_cr3();
    current_TCB->ESP = 0;
}

void switch_to_task(ThreadControlBlock* next) {
    #ifdef DBG_TASKSWITCH
    kprintf("Switching to Task: %d (%s)\n", next->id, next->meta_data->name.c_str());
    #endif
    // RegisterDump* regs = (RegisterDump*) next->ESP0;
    // void* addr = &(regs->eip);
    // VirtualAddress temp_addr = MemoryManager::the().temp_map((u32)addr-(u32)addr%PAGE_SIZE);
    // RegisterDump* temp_regs = (RegisterDump*) temp_addr;
    // u32 val = temp_regs->eip;
    // MemoryManager::the().un_temp_map();
    // kprintf("returning to: 0x%x\n", val);
    asm_switch_to_task(next);
}

#define CLONE_PAGE_DIRECTORY

constexpr size_t NUM_PAGES_PER_STACK = 10;

ThreadControlBlock* create_kernel_task(void (*func)()) {
    ThreadControlBlock* tcb = new ThreadControlBlock();
    tcb->id = ++current_thread_id;
    // allocate stack space for new task
    kprintf("new CR3: 0x%x\n", (u32)tcb->CR3);
    for(size_t i = 0; i < NUM_PAGES_PER_STACK; ++i)
    {
        MemoryManager::the().allocate((u32)((u32)next_task_stack_virtual_addr + i*PAGE_SIZE),
            PageWritable::YES,
            UserAllowed::NO);

    }
    next_task_stack_virtual_addr = (void*)((u32)next_task_stack_virtual_addr + PAGE_SIZE*NUM_PAGES_PER_STACK);

    u32* new_stack = (u32*)((u32)next_task_stack_virtual_addr - 4);

    next_task_stack_virtual_addr = (void*)((u32)next_task_stack_virtual_addr + PAGE_SIZE); // leave a blank unmapped page to detect stack overflows

    // initialize value of stack (will be popped off at the end of switch_to_task)
    stack_push(&new_stack, u32(Scheduler::terminate_current)); // jumps here after func returns
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
    tcb->CR3 = (void*) (u32)MemoryManager::the().clone_page_directory(CopyUserPages::NO).get_base();
    #else
    tcb->CR3 = (void*) (u32)get_cr3();
    #endif
    tcb->ESP = new_stack;
    tcb->ESP0 = new_stack;

    return tcb;
}