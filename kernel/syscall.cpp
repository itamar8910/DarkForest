#include "syscall.h"
#include "cpu.h"
#include "logging.h"
#include "asserts.h"
#include "sleep.h"
#include "Scheduler.h"
#include "MM/MemoryManager.h"
#include "FileSystem/FileUtils.h"
#include "fork_args.h"

// #define DBG_SYSCALL
// #define DBG_SYSCALL2


u32 syscalls_gate(u32 syscall_idx, u32, u32, u32);

void print_stack(u32 esp, size_t num) {
    kprintf("esp: 0x%x\n", esp);
    for(size_t i = 0; i < num; i++) {
        kprintf("0x%x: 0x%x\n",esp+(i*4) ,*(u32*)(esp+(i*4)));
    }
}

ISR_HANDLER(syscall);
void isr_syscall_handler(RegisterDump& regs) {
#ifdef DBG_SYSCALL
    if(regs.eax != Syscall::Kputc) {
        kprintf("task: %s - syscall : %d\n", Scheduler::the().current().name().c_str(), regs.eax);
    }
#endif
    regs.eax = syscalls_gate(regs.eax, regs.ebx, regs.ecx, regs.edx);
#ifdef DBG_SYSCALL2
    kprintf("task: %s - done syscall\n", Scheduler::the().current_task().meta_data->name.c_str());
    kprintf("eip: 0x%x\n", regs.eip);
    kprintf("kernel stack:\n");
    print_stack(regs.esp, 16);
    kprintf("user stack:\n");
    print_stack(regs.useresp, 16);
#endif
}

void syscall_exit(int code) {
    kprintf("process exited with code: %d\n", code);
    Scheduler::the().terminate_current();
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
            return Scheduler::the().current().pid();
        case Syscall::Exit:
            syscall_exit((int)arg1);    
            return 0;
        case Syscall::Kputc:
            kprintf("%c", (char)arg1);
            return 0;
        case Syscall::AllocatePage:
            MemoryManager::the().allocate(arg1, PageWritable::YES, UserAllowed::YES);
            return 0;
        case Syscall::Open:
            return Scheduler::the().current().syscall_open(String((char*) arg1));
        case Syscall::IOCTL:
            return Scheduler::the().current().syscall_ioctl(arg1, arg2, (void*) arg3);
        case Syscall::FILE_SIZE:
            return Scheduler::the().current().syscall_file_size(arg1);
        case Syscall::READ:
            return Scheduler::the().current().syscall_read(arg1, (char*) arg2, arg3);
        case Syscall::WRITE:
            return Scheduler::the().current().syscall_write(arg1, (char*) arg2, arg3);
        case Syscall::ForkAndExec:
            {
            return Scheduler::the().current().syscall_ForkAndExec((ForkArgs*)arg1);
            }
        case Syscall::WAIT:
            return Scheduler::the().current().syscall_wait(arg1);
        case Syscall::ListDir:
            return Scheduler::the().current().syscall_listdir(String((char*) arg1), (char*) arg2, (size_t*) arg3);
        case Syscall::SetCurrentDirectory:
            return Scheduler::the().current().syscall_set_current_directory(String((char*) arg1));
        case Syscall::GetCurrentDirectory:
            return Scheduler::the().current().syscall_get_current_directory((char*) arg1, (size_t*) arg2);
        case Syscall::CreateFile:
            return Scheduler::the().current().syscall_creste_entry((char*)arg1, DirectoryEntry::Type::File);
        case Syscall::CreateDirectory:
            return Scheduler::the().current().syscall_creste_entry((char*)arg1, DirectoryEntry::Type::Directory);
        default:
            kprintf("invalid syscall: %d\n", syscall_idx);
            ASSERT_NOT_REACHED();
    }
    return 0;
}

void init_syscalls() {
    register_interrupt_handler(SYSCALL_ISR_IDX, isr_syscall_entry, true);
}
