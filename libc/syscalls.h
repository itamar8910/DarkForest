#pragma once
#include "types.h"

namespace Syscall {
    enum SyscallIdx {
        SleepMs=1,
        Kprintf=2,
        DbgPrint=3,
        getID=4,
        Exit=5,
        Kputc=6,
    };

    int invoke(SyscallIdx syscall, u32 arg);
};

