#include "kmalloc.h"
#include "types.h"
#include "assert.h"

void* KMALLOC_START = (void*) (3 * MB);
void* KMALLOC_END = (void*) (4 * MB);

/*
Initial implementation: advance next_free pointer,
does not support freeing memory
*/
void* next_free = (void*) KMALLOC_START; 

void* kmalloc(size_t size) {
    void* cur = next_free;
    next_free = (void*)((size_t)next_free + size);
    ASSERT(next_free < KMALLOC_END, "kmalloc ran out of memory");
    return cur;
}

void* operator new(size_t size) {
    return kmalloc(size);
}

void* operator new[](size_t size) {
    return kmalloc(size);
}