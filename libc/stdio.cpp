#include "stdio.h"
#include "syscalls.h"
#include "stdarg.h"
#include "printf.h"
#include "df_unistd.h"
#include "cstring.h"

#ifdef __cplusplus
extern "C" {
#endif

void puts(const char* str) {
    std::write(STDOUT, str, strlen(str));
    putc('\n');
}

void putc(char c) {
    putchar(c);
}

int putchar(int c)
{
    return std::write(STDOUT, (char*)&c, 1);
}

void kputc(char c) {
    Syscall::invoke(Syscall::Kputc, c);
}

void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf_internal(putc, fmt, args);
    va_end(args);

}


int getchar() {
    char c;
    int rc = std::read(STDIN, &c, 1);
    if(rc < 0) {
        return rc;
    }
    return c;
}

int fclose(FILE *)
{
    ASSERT_NOT_REACHED();
}
FILE *fopen(const char *, const char *)
{
    ASSERT_NOT_REACHED(); }

int fprintf(FILE *, const char *, ...)
{
    ASSERT_NOT_REACHED();
}
size_t fread(void *, size_t , size_t , FILE *)
{
    ASSERT_NOT_REACHED();
}
size_t fwrite(const void *, size_t , size_t , FILE *)
{
    ASSERT_NOT_REACHED();
}
int fseek(FILE *, long , int )
{
    ASSERT_NOT_REACHED();
}
long ftell(FILE *)
{
    ASSERT_NOT_REACHED();
}
int fflush(FILE *)
{
    ASSERT_NOT_REACHED();
}

// code for math functions taken from here:
// https://gitlab.incom.co/CM-Shield/u-boot/commit/aa7839b39c2ee77f9ab8c393c56b8d812507dbb7
// https://github.com/zayac/qemu-arm/blob/master/qemu/roms/ipxe/src/libgcc/__udivmoddi4.c
// https://code.woboq.org/llvm/compiler-rt/lib/builtins/divdi3.c.html

int64_t __divdi3(int64_t a, int64_t b)
{
    const int bits_in_dword_m1 = (int)(sizeof(int64_t) * 8) - 1;
    int64_t s_a = a >> bits_in_dword_m1;                   // s_a = a < 0 ? -1 : 0
    int64_t s_b = b >> bits_in_dword_m1;                   // s_b = b < 0 ? -1 : 0
    a = (a ^ s_a) - s_a;                                   // negate if s_a == -1
    b = (b ^ s_b) - s_b;                                   // negate if s_b == -1
    s_a ^= s_b;                                            // sign of quotient
    return (__udivmoddi4(a, b, (uint64_t*)0) ^ s_a) - s_a; // negate if s_a == -1
}

uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint64_t* rem_p)
{
    uint64_t quot = 0, qbit = 1;

    if (den == 0) {
        return 1 / ((unsigned)den); /* Intentional divide by zero, without
				 triggering a compiler warning which
				 would abort the build */
    }

    /* Left-justify denominator and count shift */
    while ((int64_t)den >= 0) {
        den <<= 1;
        qbit <<= 1;
    }

    while (qbit) {
        if (den <= num) {
            num -= den;
            quot += qbit;
        }
        den >>= 1;
        qbit >>= 1;
    }

    if (rem_p)
        *rem_p = num;

    return quot;
}

int remove(const char *)
{
    ASSERT_NOT_REACHED();
}

int rename(const char *, const char *)
{
    ASSERT_NOT_REACHED();
}

int snprintf(char *, size_t , const char *, ...)
{
    ASSERT_NOT_REACHED();
}

int sscanf(const char *, const char *, ...)
{
    ASSERT_NOT_REACHED();
}

int vprintf(const char *, va_list)
{
    ASSERT_NOT_REACHED();
}

int vsnprintf(char * str, size_t size, const char * fmt, va_list args)
{
    size_t char_index = 0;
    auto putc_func = [&](char c) {
        if (char_index < size) {
            str[char_index] = c;
        }
        ++char_index;
    };

    printf_internal(putc_func, fmt, args);

    if (char_index < size) {
        str[char_index] = '\0';
    } else if (size > 0) {
        str[size - 1] = '\0';
    }

    return char_index;
}

int vfprintf(FILE *, const char *, va_list)
{
    ASSERT_NOT_REACHED();
}

#ifdef __cplusplus
}
#endif

void kprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf_internal(kputc, fmt, args);
    va_end(args);

}
