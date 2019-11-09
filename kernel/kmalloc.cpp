#include "kmalloc.h"
#include "types.h"
#include "asserts.h"
#include "MM/MM_types.h"
#include "MemoryManager.h"
#include "new.h"

// #define KMALLOC_DBG

KMallocMode kmalloc_mode = KMallocMode::KMALLOC_ETERNAL;

void* KMALLOC_ETERNAL_START = (void*) (3 * MB);
void* KMALLOC_ETERNAL_END = (void*) (4 * MB - PAGE_SIZE);
void* KERNEL_HEAP_START = (void*) 0xc1000000;
void* KERNEL_HEAP_MAX_END = (void*) 0xc5000000;
void* eternal_next_free = (void*) KMALLOC_ETERNAL_START; 



void* kmalloc_eternal(size_t size) {
    void* cur = eternal_next_free;
    eternal_next_free = (void*)((size_t)eternal_next_free + size);
    ASSERT(eternal_next_free < KMALLOC_ETERNAL_END, "kmalloc ran out of memory");
    return cur;
}


void kmalloc_set_mode(KMallocMode mode) {
    kmalloc_mode = mode;
}

void KernelHeapAllocator::allocate_page(void* addr) {
    MemoryManager::the().allocate((u32)addr, true, false);
}

static KernelHeapAllocator* s_the = nullptr;

void KernelHeapAllocator::initialize() {
    kprintf("KMalloc::initialize\n");
    // allocate first page of kernel heap

    s_the = (KernelHeapAllocator*) kmalloc_eternal(sizeof(KernelHeapAllocator));
    MemoryManager::the().allocate((u32)KERNEL_HEAP_START, true, false);
    new(s_the) KernelHeapAllocator(KERNEL_HEAP_START, PAGE_SIZE);
}

KernelHeapAllocator& KernelHeapAllocator::the() {
    ASSERT(s_the != nullptr, "KMalloc not initialized");
    return *s_the;
}

void* kmalloc(size_t size) {
    return KernelHeapAllocator::the().allocate(size);
}

void kfree(void* addr) {
    KernelHeapAllocator::the().free(addr);
}


void* new_wrapper(size_t size) {
    switch(kmalloc_mode) {
        case KMallocMode::KMALLOC_ETERNAL:
            return kmalloc_eternal(size);
        case KMallocMode::KMALLOC_NORMAL:
            return kmalloc(size);
    }
    ASSERT_NOT_REACHED("new: invalid malloc mode");
    return 0;
}

void free_wrapper(void* addr) {
    switch(kmalloc_mode) {
        case KMallocMode::KMALLOC_ETERNAL:
            ASSERT_NOT_REACHED("cannot free memory in KMALLOC_ETERNAL mode");
            return;
        case KMallocMode::KMALLOC_NORMAL:
            kfree(addr);
            return;
    }
    ASSERT_NOT_REACHED("new: invalid free mode");
}

void* operator new(size_t size) {
    return new_wrapper(size);
}

void* operator new[](size_t size) {
    return new_wrapper(size);
}

void operator delete(void* addr) {
    free_wrapper(addr);
}

void operator delete[](void* addr) {
    free_wrapper(addr);
}