
#include "string.h"
#include "logging.h"
#include "types.h"
#include "asserts.h"
extern "C"
{
size_t strlen(const char* str) 
{
	#ifdef ASSERTS_LEVEL_1
	ASSERT(str != nullptr);
	#endif
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

void* memset(void* s, int c, size_t n) {
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
	size_t i;
	for(i = 0; src[i]; i++) {
		dest[i] = src[i];
	}
	dest[i] = 0;

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

int memcmp(const void *s1, const void *s2, size_t n) {
	char* p1 = (char*) s1;
	char* p2 = (char*) s2;
	for(;n>0;n--) {
		if(*(p1++) != *(p2++))
			return *(p1-1) - *(p2-1);
	}
	return 0;
}

int strcmp(const char *s1, const char *s2) {
	char* p1 = (char*) s1;
	char* p2 = (char*) s2;
	for(
		; (*p1) == (*p2) && (*p1) ; p1++, p2++
	);
	return *p1 - *p2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
	char* p1 = (char*) s1;
	char* p2 = (char*) s2;
	for( size_t i = 0
		; (*p1) == (*p2) && (*p1) && i < n-1
		; p1++, p2++, i++
	);
	return *p1 - *p2;

}

}
void ascii_to_unicode(char* dst, const char* src, size_t num_chars)
{
	for(size_t i = 0; i < num_chars; ++i)
	{
		dst[i*2] = src[i];
		dst[(i*2)+1] = 0;
	}
}