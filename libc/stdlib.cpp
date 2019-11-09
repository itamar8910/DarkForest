#include "stdlib.h"

int atoi(const char* str) {
    int sum = 0;
    int sign = 1;
    if(*str == '-') {
        str++;
        sign = -1;
    }
    while(*str) {
        sum *= 10;
        sum += *str - '0';
        str++;
    }
    return sum * sign;
}


#ifndef KERNEL
#include "types.h"
#include "unistd.h"
void* operator new(size_t size) {
    (void)size;
    exit(CODE_ASSERT_NOT_REACHED);
    return (void*)0xdeadbeef;
}

void* operator new[](size_t size) {
    (void)size;
    exit(CODE_ASSERT_NOT_REACHED);
    return (void*)0xdeadbeef;
}

void operator delete(void* addr) {
    (void)addr;
    exit(CODE_ASSERT_NOT_REACHED);
}

void operator delete[](void* addr) {
    (void)addr;
    exit(CODE_ASSERT_NOT_REACHED);
}
#endif


