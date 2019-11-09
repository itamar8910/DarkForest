#include "syscall.h"
#include "cpu.h"
#include "logging.h"
#include "asserts.h"
#include "sleep.h"
#include "Scheduler.h"

#define DBG_SYSCALL


u32 syscalls_gate(u32 syscall_idx, u32, u32, u32);

ISR_HANDLER(syscall);
void isr_syscall_handler(RegisterDump& regs) {
#ifdef DBG_SYSCALL
    kprintf("task: %s - syscall : %d\n", Scheduler::the().current_task().meta_data->name.c_str(), regs.eax);
#endif
    regs.eax = syscalls_gate(regs.eax, regs.ebx, regs.ecx, regs.edx);
}

void syscall_exit(int code) {
    kprintf("process exited with code: %d\n", code);
    Scheduler::the().terminate();
}

u32 syscalls_gate(u32 syscall_idx, u32 arg1, u32 arg2, u32 arg3) {
    switch(syscall_idx) {
        case Syscall::SleepMs:
            sleep_ms(arg1);
            return 0;
        case Syscall::Kprintf:
            kprintf((char*) arg1, arg2, arg3);
            return 0;
        case Syscall::DbgPrint:
            kprint("DbgPrint\n");
            return 0;
        case Syscall::getID:
            return Scheduler::the().current_task().id;
        case Syscall::Exit:
            syscall_exit((int)arg1);    
            return 0;
        default:
            kprintf("invalid syscall: %d\n", syscall_idx);
            ASSERT_NOT_REACHED("invalid syscall");
    }
    return 0;
}

void init_syscalls() {
    register_interrupt_handler(SYSCALL_ISR_IDX, isr_syscall_entry, true);
}