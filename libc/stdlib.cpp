#include "stdlib.h"
#include "malloc.h"
#include "asserts.h"

int atoi_10(const char* str) {
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

int atoi_16(const char* str) {
    int sum = 0;
    int sign = 1;
    if(*str == '-') {
        str++;
        sign = -1;
    }
    while(*str) {
        sum *= 16;
        char c = *str;
        if(c >= '0' && c <= '9') {
            sum += c - '0';
        } else if(c >= 'a' && c <= 'f') {
            sum += 10 + (c-'a');
        } else {
            ASSERT_NOT_REACHED("atoi_16: invalid char");
        }
        str++;
    }
    return sum * sign;
}

int atoi(const char* str, size_t base) {
    switch(base) {
        case 10:
            return atoi_10(str);
        case 16:
            return atoi_16(str);
    }
    ASSERT_NOT_REACHED("atoi: invalid base");
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


