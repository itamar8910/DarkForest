#include "malloc.h"
#include "types.h"
#include "mman.h"
#include "constants.h"
#include "asserts.h"
#include "std_misc.h"

void* USERSPACE_HEAP_START = (void*) 0x70000000;
static UserspaceHeapAllocator* s_the = nullptr;
static char mem_placeholder[sizeof(UserspaceHeapAllocator)];


void UserspaceHeapAllocator::initialize() {
    // indicate that allocate_page is in global namespace (included form mman.h)
    s_the = (UserspaceHeapAllocator*) mem_placeholder;
    ::allocate_page(USERSPACE_HEAP_START);
    new(s_the) UserspaceHeapAllocator(USERSPACE_HEAP_START, PAGE_SIZE);
}

UserspaceHeapAllocator& UserspaceHeapAllocator::the() {
    if(s_the == nullptr){
        UserspaceHeapAllocator::initialize();
    }
    return *s_the;
}

void UserspaceHeapAllocator::allocate_page(void* addr) {
    ::allocate_page(addr);
}

void* malloc(size_t size) {
    return UserspaceHeapAllocator::the().allocate(size);
}

void free(void* addr) {
    UserspaceHeapAllocator::the().free(addr);
}
