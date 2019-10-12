#include "kmalloc.h"
#include "types.h"
#include "Kassert.h"
#include "MM/MM_types.h"
#include "MemoryManager.h"
#include "new.h"

KMallocMode kmalloc_mode = KMALLOC_ETERNAL;

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

KMalloc* km = nullptr;
void KMalloc::initialize() {
    // allocate first page of kernel heap
    MemoryManager::the().allocate((u32)KERNEL_HEAP_START, true, false);

    km = (KMalloc*) kmalloc_eternal(sizeof(KMalloc));
    new(km) KMalloc(KERNEL_HEAP_START, PAGE_SIZE);
}

KMalloc::KMalloc(void* addr, u32 size) {
    // allocate first free block on heap itself
    m_first_free = (FreeBlock*) (addr);
    m_first_free->next = nullptr;
    m_first_free->addr = (void*)((u32) addr + sizeof(FreeBlock));
    m_first_free->size = size - sizeof(FreeBlock);
    m_current_heap_end = (void*) ((u32)addr + size);
}

KMalloc& KMalloc::the() {
    ASSERT(km != nullptr, "KMalloc not initialized");
    return *km;
}

void* KMalloc::allocate(u32 size) {
    kprintf("KMalloc::allocate: %d bytes\n", size);
    // loop over free list
    // find a block with size` >=  size+sizeof(FreeBlock)
    /*
        FreeBlock | memory_blob
                    ^
                    |
                    |
                    returned ptr
                    
    */

}
void KMalloc::free(void* addr) {
    kprintf("KMalloc::free: 0x%x\n", addr);

}

void* kmalloc(size_t size) {
    return KMalloc::the().allocate(size);
}

void kfree(void* addr) {
    KMalloc::the().free(addr);
}


void* new_wrapper(size_t size) {
    switch(kmalloc_mode) {
        case KMALLOC_ETERNAL:
            return kmalloc_eternal(size);
        case KMALLOC_NORMAL:
            return kmalloc(size);
    }
    ASSERT_NOT_REACHED("new: invalid kmalloc mode");
    return 0;
}

void* operator new(size_t size) {
    return new_wrapper(size);
}

void* operator new[](size_t size) {
    return new_wrapper(size);
}