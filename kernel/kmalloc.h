#pragma once

#include "types.h"
#include "HeapAllocator.h"

enum class KMallocMode {
    KMALLOC_ETERNAL,
    KMALLOC_NORMAL
};


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

/**
 * allocate memory that cannot be freed
 */
void* kmalloc_eternal(size_t size);

void kmalloc_set_mode(KMallocMode mode);

void kmalloc_initialize();
