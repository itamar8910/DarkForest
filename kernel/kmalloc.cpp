#include "kmalloc.h"
#include "types.h"
#include "asserts.h"
#include "MM/MM_types.h"
#include "MM/MemoryManager.h"
#include "new.h"

// #define KMALLOC_DBG

void* KERNEL_HEAP_START = (void*)   0xc1000000;
void* KERNEL_HEAP_MAX_END = (void*) 0xc5000000;


void KernelHeapAllocator::allocate_page(void* addr) {
    MemoryManager::the().allocate((u32)addr, PageWritable::YES, UserAllowed::NO);
}

static KernelHeapAllocator* s_the = nullptr;

static u8 kernel_heap_allocator_memory[sizeof(KernelHeapAllocator)];

void KernelHeapAllocator::initialize() {
    kprintf("KMalloc::initialize\n");
    // allocate first page of kernel heap

    s_the = (KernelHeapAllocator*) kernel_heap_allocator_memory;
    MemoryManager::the().allocate((u32)KERNEL_HEAP_START, PageWritable::YES, UserAllowed::NO);
    new(kernel_heap_allocator_memory) KernelHeapAllocator(KERNEL_HEAP_START, PAGE_SIZE);
}

KernelHeapAllocator& KernelHeapAllocator::the() {
    ASSERT(s_the != nullptr);
    return *s_the;
}

void* kmalloc(size_t size) {
    void* p =  KernelHeapAllocator::the().allocate(size);
    // kprintf("KMalloc::kmalloc: 0x%x\n", p);
    return p;
}

void kfree(void* addr) {
    KernelHeapAllocator::the().free(addr);
}


void* new_wrapper(size_t size) {
    ASSERT(s_the != nullptr);
    return kmalloc(size);
}

void free_wrapper(void* addr) {
    ASSERT(s_the != nullptr);
    kfree(addr);
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