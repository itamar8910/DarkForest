#pragma once

#include "types.h"

enum KMallocMode {
    KMALLOC_ETERNAL,
    KMALLOC_NORMAL
};

struct FreeBlock {
    FreeBlock* next;
    void* addr;
    u32 size;
};

class KMalloc {
    
public:
    static void initialize();
    static KMalloc& the();

    void* allocate(u32 size);
    void free(void* addr);

private:
    KMalloc(void* addr, u32 size);

    FreeBlock* m_first_free;
    void* m_current_heap_end;
};

void* kmalloc(size_t size);

/**
 * allocate memory that cannot be freed
 */
void* kmalloc_eternal(size_t size);

void kmalloc_set_mode(KMallocMode mode);

void kmalloc_initialize();
