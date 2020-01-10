#include "unistd.h"
#include "stdio.h"
#include "asserts.h"

#ifdef KERNEL
static_assert(0);
#endif


int main() {
    kprintf("hello, world!\n");
    // generate a segfault
    // char* p = nullptr;
    // *p = 1;
    return 0;
}