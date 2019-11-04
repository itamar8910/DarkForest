#include "syscall.h"
#include "cpu.h"
#include "logging.h"
#include "Kassert.h"
#include "sleep.h"

void syscall_sleep_ms(u32 ms) {
    kprintf("syscall sleep_ms: %d\n", ms);
    sleep_ms(ms);
}

void syscall_printk(char* msg) {
    kprint(msg);
}

void syscall_print_dbg() {
    kprint("syscall_print_dbg\n");
}

ISR_HANDLER(syscall);
void isr_syscall_handler(RegisterDump& regs) {
    kprintf("ISR syscall handler. eax: %d\n", regs.eax);
    switch(regs.eax) {
        case 1: 
            kprintf("eax: 0\n");
            syscall_sleep_ms(regs.ecx);
            break;
        case 2: 
            syscall_printk((char*) regs.ecx);
            break;
        case 3: 
            syscall_print_dbg();
            break;
        default:
            kprintf("invalid syscall: %d\n", regs.eax);
            ASSERT_NOT_REACHED("invalid syscall");
    }
    // TODO: do we need to set TSS.ESP0 before returning to usermode?
    // cpu_hang();
}

void init_syscalls() {
    register_interrupt_handler(SYSCALL_ISR_IDX, isr_syscall_entry, true);
}