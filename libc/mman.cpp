#include "mman.h"

void allocate_page(void* addr) {
    Syscall::invoke(Syscall::AllocatePage, (u32)addr);
}