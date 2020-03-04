#pragma once

#include "types.h"
#include "HeapAllocator.h"


class KernelHeapAllocator : public HeapAllocator {
public:
    static void initialize();
    static KernelHeapAllocator& the();
protected:
    void allocate_page(void* addr) override;
private:
    KernelHeapAllocator(void* addr, u32 size)
        : HeapAllocator(addr, size) {}
};

void* kmalloc(size_t size);
void kfree(void* addr);

void kmalloc_initialize();
