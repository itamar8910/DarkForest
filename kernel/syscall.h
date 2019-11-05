#pragma once

#define SYSCALL_ISR_IDX 0x80

namespace Syscall {
    enum SyscallIdx {
        Sleep=1,
        Kprintf=2,
        DbgPrint=3,
        getID=4,
    };
};

void init_syscalls();