#include "syscall.h"
#include "cpu.h"
#include "logging.h"

ISR_HANDLER(syscall);
void isr_syscall_handler(RegisterDump& regs) {
    kprintf("ISR syscall handler. eax: %d\n", regs.eax);
    // cpu_hang();
}

void init_syscalls() {
    register_interrupt_handler(SYSCALL_ISR_IDX, isr_syscall_entry, true);
}