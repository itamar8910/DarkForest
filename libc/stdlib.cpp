#include "stdlib.h"
#include "malloc.h"
#include "asserts.h"
#include "printf.h"
#include "Math.h"
#include "string.h"

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
            ASSERT_NOT_REACHED();
        }
        str++;
    }
    return sum * sign;
}

int df_atoi(const char* str, size_t base) {
    switch(base) {
        case 10:
            return atoi_10(str);
        case 16:
            return atoi_16(str);
    }
    ASSERT_NOT_REACHED();
}

int atoi(const char* str) {
    return atoi_10(str);
}

int itoa(int num, char* out, u32 out_len)
{
    u32 out_idx = 0;
    auto putchar = [&](char c)
    {
        if(out_idx >= out_len)
        {
            return;
        } 
        out[out_idx++] = c;
    };
    const int rc = printf_internal_wrapper(putchar, "%d", num);
    if(out_idx >= out_len)
    {
        return 0;
    }
    out[out_idx] = 0;
    return rc;

}


#ifndef KERNEL
#include "types.h"
#include "df_unistd.h"
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

extern "C" {

int abs(int j)
{
    return j >= 0 ? j : -j;
}

double atof(const char *nptr)
{
    (void)nptr;
    ASSERT_NOT_REACHED();
}

void *calloc(size_t nmemb, size_t size)
{
    void* res = malloc(nmemb * size);
    if (!res)
        return res;
    memset(res, 0, nmemb*size);
    return res;
}

void *realloc(void * ptr, size_t size)
{
    // return nullptr;
    if (!size) {
        free(ptr);
        return nullptr;
    }

    auto new_ptr = malloc(size);
    ASSERT(new_ptr);
    if (!ptr)
        return new_ptr;
    
    

    u32 old_size = UserspaceHeapAllocator::the().size_of_block(ptr);
    ASSERT(old_size);

    memcpy(new_ptr, ptr, Math::min(old_size, size));
    free(ptr);
    return new_ptr;
}

}
