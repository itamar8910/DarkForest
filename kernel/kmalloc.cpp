#include "kmalloc.h"
#include "types.h"
#include "Kassert.h"
#include "MM/MM_types.h"
#include "MemoryManager.h"
#include "new.h"

// #define KMALLOC_DBG

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
    kprintf("KMalloc::initialize\n");
    // allocate first page of kernel heap

    km = (KMalloc*) kmalloc_eternal(sizeof(KMalloc));
    MemoryManager::the().allocate((u32)KERNEL_HEAP_START, true, false);
    new(km) KMalloc(KERNEL_HEAP_START, PAGE_SIZE);
}

KMalloc::KMalloc(void* addr, u32 size) {
    // allocate first free block on heap itself
    m_first_free = MemBlock::initialize(addr,
                                    nullptr, 
                                    (void*)((u32) addr + sizeof(MemBlock)),
                                    size - sizeof(MemBlock)
                                    );
    m_current_heap_end = (void*) ((u32)addr + size);
}

KMalloc& KMalloc::the() {
    ASSERT(km != nullptr, "KMalloc not initialized");
    return *km;
}

void* KMalloc::allocate(u32 size) {
    #ifdef KMALLOC_DBG
    kprintf("KMalloc::allocate: %d bytes", size);
    #endif
    // loop over free list
    // find a block with size` >=  size+sizeof(FreeBlock)
    /*
        FreeBlock | memory_blob
                    ^
                    |
                    |
                    returned ptr
                    
    */
    bool retried = false;
find_block:
   MemBlock* current;
   for(current = m_first_free; current != nullptr; current=current->next) {
       // TODO: support >=
       // what if we completely empty the MemBlock?
       if(current->size > size + sizeof(MemBlock)) {
           break;
       }
   }
   if(current == nullptr) {
       ASSERT(!retried, "allocate: only retry once");
       exapnd_heap((size / PAGE_SIZE) + 1);
       retried = true;
       goto find_block; // sorry
   }

    auto new_block = MemBlock::initialize(
        current->addr,
        nullptr,
        (void*)((u32)current->addr + sizeof(MemBlock)),
        size
    );
    new_block->magic = MAGIC_USED;

    current->addr = (MemBlock*) ((u32)current->addr + sizeof(MemBlock) + size);
    current->size -= sizeof(MemBlock) + size;

    #ifdef KMALLOC_DBG
    kprintf("- 0x%x\n", new_block->addr);
    #endif
    return new_block->addr;
}

void KMalloc::exapnd_heap(u32 num_pages) {
    kprintf("KMalloc:: expanding heap by %d pages\n", num_pages);
    
    for(u32 i = 0; i < num_pages; i++) {
        MemoryManager::the().allocate((u32)m_current_heap_end + i*PAGE_SIZE, true, false);
    }

    auto new_block = MemBlock::initialize(
        m_current_heap_end,
        nullptr,
        (void*)((u32) m_current_heap_end + sizeof(MemBlock)),
        num_pages * PAGE_SIZE - sizeof(MemBlock)
    );
    m_current_heap_end = (void*)((u32)m_current_heap_end + num_pages * PAGE_SIZE);

    add_mem_block(new_block);
}

void KMalloc::add_mem_block(MemBlock* block) {
    ASSERT(block->is_magic_free(), "KMalloc::add_mem_block - bad block magic");

    // try to merge this chunk with an existing block in the free list

    // NOTE: we are possibly looping over many memory chunks here,
    // we could try to store the free memory blocks in a search tree 
    // to decrease runtime penalty of consolidating the memory blocks

    for(MemBlock* cur = m_first_free; cur != nullptr; cur = cur->next) {
        if(
            (u32)block->addr + block->size == (u32)cur->addr
        ) {
            cur->size += block->size + sizeof(MemBlock);
            cur->addr = (void*)((u32)block->addr - sizeof(MemBlock));
            return;
        }
    }

    block->next = m_first_free;
    m_first_free = block;
}

u32 KMalloc::current_free_space(u32& num_blocks) {
    num_blocks = 0;
    u32 free_bytes = 0;
    for(MemBlock* cur = m_first_free; cur != nullptr; cur = cur->next, ++num_blocks) {
        free_bytes += cur->size;
    }
    return free_bytes;
}

void KMalloc::free(void* addr) {
    #ifdef KMALLOC_DBG
    kprintf("KMalloc::free: 0x%x", addr);
    #endif
    // MemBlock should be stored before address
    MemBlock* block = (MemBlock*)((u32)addr - sizeof(MemBlock));
    ASSERT(block->is_magic_used(), "KMalloc::free - bad block magic - double free / corrupted magic");
    #ifdef KMALLOC_DBG
    kprintf("- %d bytes\n", block->size);
    u32 n_blocks = 0;
    u32 free_heapspace = current_free_space(n_blocks);
    kprintf("heap space: %d bytes, #blocks: %d\n", free_heapspace, n_blocks);
    #endif
    block->magic = MAGIC_FREE;
    add_mem_block(block);
}

MemBlock* MemBlock::initialize(void* struct_addr,
                                MemBlock* next,
                                void* addr,
                                u32 size) {
    MemBlock* block = (MemBlock*) struct_addr;
    block->magic = MAGIC_FREE;
    block->next = next;
    block->addr = addr;
    block->size = size;
    return block;
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
    ASSERT_NOT_REACHED("new: invalid malloc mode");
    return 0;
}

void free_wrapper(void* addr) {
    switch(kmalloc_mode) {
        case KMALLOC_ETERNAL:
            ASSERT_NOT_REACHED("cannot free memory in KMALLOC_ETERNAL mode");
            return;
        case KMALLOC_NORMAL:
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