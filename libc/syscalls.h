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
        AllocatePage=7,
        Open=8,
        Close=9,
        IOCTL=10,
        FILE_SIZE=11,
        READ=12,
        WRITE=13,
    };

    int invoke(SyscallIdx syscall, u32 arg1=0, u32 arg2=0, u32 arg3=0);
};

