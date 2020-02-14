#pragma once
#include "types.h"

namespace Syscall {
    enum SyscallIdx {
        SleepMs=1,
        Kprintf,
        DbgPrint,
        GetID,
        Exit,
        Kputc,
        AllocatePage,
        Open,
        Close,
        IOctl,
        FileSize,
        Read,
        Write,
        ForkAndExec,
        Wait,
        ListDir,
        SetCurrentDirectory,
        GetCurrentDirectory,
        CreateFile,
        CreateDirectory,
        IsFile,
        IsDirectory,
        CreateSharedMemory,
        OpenSharedMemory,
        SendMessage,
        GetMessage,
        GetPidByName,
        MapDevice,
        GenerateGUID,
    };

    int invoke(SyscallIdx syscall, u32 arg1=0, u32 arg2=0, u32 arg3=0);
};

