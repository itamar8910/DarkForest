#pragma once
#include "types.h"

#define MAGIC_FREE 0xc001beef
#define MAGIC_USED 0xdeadbeef

/**
 * Basic free list
 * each allocated memory block is preceded by a MemBlock structure
 * 
*/


struct MemBlock {
    u32 magic; // used to avoid corrupted blocks
    MemBlock* next;
    void* addr;
    u32 size;

    static MemBlock* initialize(void* strct_addr,
                                          MemBlock* next,
                                          void* addr,
                                          u32 size);
    bool is_magic_free() {return magic == MAGIC_FREE;}
    bool is_magic_used() {return magic == MAGIC_USED;}
    void assert_valid_magic();
};

class HeapAllocator {
public:

    void* allocate(u32 size);
    void free(void* addr);

    u32 current_free_space(u32& num_blocks);
    u32 current_free_space() {u32 x = 0; return current_free_space(x);}

protected:
    HeapAllocator(void* addr, u32 size);
    void exapnd_heap(u32 num_pages);
    void add_mem_block(MemBlock*);

    virtual void allocate_page(void* addr) = 0;

    MemBlock* m_first_free;
    void* m_current_heap_end;
};