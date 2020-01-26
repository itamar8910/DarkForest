#include "HeapAllocator.h"
#include "types.h"
#include "asserts.h"
#include "constants.h"
#include "logging.h"

#ifdef KERNEL
#include "kernel/lock.h"
#include "kernel/InterruptDisabler.h"
#endif

#ifdef KERNEL
#include "logging.h"
#endif
#ifdef USERSPACE
#include "stdio.h"
#endif

void MemBlock::assert_valid_magic() {
    if(magic != MAGIC_FREE &&  magic != MAGIC_USED) {
        dbgprintf("MemBlock invalid magic: 0x%x", magic);
    }
}

HeapAllocator::HeapAllocator(void* addr, u32 size) {
    // allocate first free block on heap itself
    m_first_free = MemBlock::initialize(addr,
                                    nullptr, 
                                    (void*)((u32) addr + sizeof(MemBlock)),
                                    size - sizeof(MemBlock)
                                    );
    m_current_heap_end = (void*) ((u32)addr + size);
    m_heap_start = addr;
}

#ifdef KERNEL
// Lock& get_heap_lock()
// {
//     static Lock lock("Heap");
//     return lock;
// }
#endif

void* HeapAllocator::allocate(u32 size) {
    // loop over free list
    // find a block with size` >=  size+sizeof(FreeBlock)
    /*
        FreeBlock | memory_blob
                    ^
                    |
                    |
                    returned ptr
                    
    */
   #ifdef KERNEL
//    LOCKER(get_heap_lock());
    InterruptDisabler dis;
   #endif
   ASSERT(size < 10000 * PAGE_SIZE);
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
       ASSERT(!retried);
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

void HeapAllocator::exapnd_heap(u32 num_pages) {
    kprintf("Malloc:: expanding heap by %d pages\n", num_pages);
    
    for(u32 i = 0; i < num_pages; i++) {
        this->allocate_page((void*)((u32)m_current_heap_end + i*PAGE_SIZE));
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

void HeapAllocator::add_mem_block(MemBlock* block) {
    ASSERT(block->is_magic_free());

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

u32 HeapAllocator::current_free_space(u32& num_blocks) {
    num_blocks = 0;
    u32 free_bytes = 0;
    for(MemBlock* cur = m_first_free; cur != nullptr; cur = cur->next, ++num_blocks) {
        free_bytes += cur->size;
    }
    return free_bytes;
}

void HeapAllocator::free(void* addr) {
   #ifdef KERNEL
//    LOCKER(get_heap_lock());
    InterruptDisabler dis;
   #endif
    // MemBlock should be stored before address
    MemBlock* block = (MemBlock*)((u32)addr - sizeof(MemBlock));
    block->assert_valid_magic();
    if(!block->is_magic_used()) {
        dbgprintf("magic: 0x%x\n", block->magic);
    }
    ASSERT(block->is_magic_used());
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

void HeapAllocator::heap_statistics(u32& free_space, u32& num_blocks, u32& allocated_pages)
{
    free_space = current_free_space(num_blocks);
    allocated_pages = ((u32)m_current_heap_end - (u32)m_heap_start) / PAGE_SIZE;
}