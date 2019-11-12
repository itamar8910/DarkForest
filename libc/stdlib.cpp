#include "stdlib.h"
#include "malloc.h"

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
    return malloc(size);
}

void* operator new[](size_t size) {
    return malloc(size);
}

void operator delete(void* addr) {
    free(addr);
}

void operator delete[](void* addr) {
    free(addr);
}
#endif


