
#include "string.h"
#include "logging.h"
#include "types.h"

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

void* memset(void* s, int c, size_t n) {
	kprintf("memset: 0x%x, %d, %d\n", s, c, n);
	// char* p = (char*)((char*)s+n-1);
	// for(; (u32)p >= (u32)s; p--) {
	// for(; (void*)p != s; p--) {
	// // kprintf("memset iter\n");
	// 	*p = c;
	// }
	char* p = (char*)s;
	for(size_t i = 0; i < n; i++) {
		p[i] = c;
	}
	return s;
}

void* memcpy(void* dest, const void* src, size_t n) {
	char* ps = (char*) src;
	char* pd = (char*) dest;
	for(; n > 0; n--) {
		*(pd++) = *(ps++);
	}
	return dest;
}

void* strcpy(char* dest, const char* src) {
	for(size_t i = 0; src[i]; i++) {
		dest[i] = src[i];
	}
	return dest;
}

void* strncpy(char* dest, const char* src, size_t n) {
	size_t i;
	for(i = 0; i < n && src[i]; i++) {
		dest[i] = src[i];
	}
	for(; i < n; i++) {
		dest[i] = 0;
	}
	return dest;
}