
#pragma once

#include "types.h"
#include "stdlib.h"
#include "HeapAllocator.h"

class UserspaceHeapAllocator : public HeapAllocator {
public:
    static void initialize();
    static UserspaceHeapAllocator& the();
    UserspaceHeapAllocator(void* addr, u32 size)
        : HeapAllocator(addr, size) {}
protected:
    void allocate_page(void* addr) override;
};

