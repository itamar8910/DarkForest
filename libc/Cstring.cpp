
#include "Cstring.h"

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

void* memset(void* s, int c, size_t n) {
	char* p = (char*)((char*)s+n);
	for(; (void*)p != s; p--) {
		*p = c;
	}
	return s;
}

void* memcpy(void* dest, const void* src, size_t n) {
	char* ps = (char*) src;
	char* pd = (char*) dest;
	for(; n > 0; n--) {
		*(ps++) = *(pd++);
	}
	return dest;
}