
#pragma once

#include "types.h"
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


void* malloc(size_t size);
void free(void* addr);