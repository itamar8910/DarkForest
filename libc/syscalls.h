#pragma once

namespace Syscall {
    enum SyscallIdx {
        SleepMs=1,
        Kprintf=2,
        DbgPrint=3,
        getID=4,
        Exit=5,
    };
};