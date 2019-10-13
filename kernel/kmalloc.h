#pragma once

#include "types.h"

enum KMallocMode {
    KMALLOC_ETERNAL,
    KMALLOC_NORMAL
};

#define MAGIC_FREE 0xc001beef
#define MAGIC_USED 0xdeadbeef

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
                        

};

class KMalloc {
    
public:
    static void initialize();
    static KMalloc& the();

    void* allocate(u32 size);
    void free(void* addr);

private:
    KMalloc(void* addr, u32 size);
    void exapnd_heap(u32 num_pages);
    void add_mem_block(MemBlock*);

    MemBlock* m_first_free;
    void* m_current_heap_end;
};

void* kmalloc(size_t size);

/**
 * allocate memory that cannot be freed
 */
void* kmalloc_eternal(size_t size);

void kmalloc_set_mode(KMallocMode mode);

void kmalloc_initialize();
