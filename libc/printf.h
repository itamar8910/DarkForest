#pragma once

#include "stdarg.h"
#include "asserts.h"
#include "stdlib.h"
#include "types.h"
#include "stdlib.h"

const char digits_dec[] = "0123456789";
const char digits_hex[] = "0123456789abcdef";

template <int Base, typename PutcFunc>
int print_int(PutcFunc putc_f, const char digits[], uint32_t val, int min_chars , char fill_char , int is_signed=false) {
    int ret = 0;
    if(val == 0) {
        putc_f('0');
        return 1;
    }
    int sign_positive = (!is_signed || (int)val > 0); // sign positive flag
    if(!sign_positive) { // if negative
        val = (uint32_t)((int)val * -1);
    }
    int n_digits = 0;
    uint32_t temp = val;
    uint32_t pow = 0;
    while(temp > 0) {
        n_digits += 1;
        pow = (pow == 0) ? 1 : pow * Base;
        temp /= Base;
    }
    if(!sign_positive) {
        putc_f('-');
        ret++;
    }
    for(int i = n_digits; i < min_chars; i++) {
        putc_f(fill_char);
    }
    for(int i = n_digits - 1; i >= 0; i--, pow /= Base) {
        uint8_t dig = (val / pow);
        putc_f(digits[dig]);
        ret++;
        val = val % pow;
    }
    return ret;
}

template <typename PutcFunc>
int print_int_dec(PutcFunc putc_f, int val, int min_chars = 0, char fill_char = 0) {
    return print_int<10>(putc_f, digits_dec, val, min_chars, fill_char, true);
}

template <typename PutcFunc>
int print_int_hex(PutcFunc putc_f, uint32_t val, int min_chars = 0, char fill_char = 0) {
    return print_int<16>(putc_f, digits_hex, val, min_chars, fill_char);
}

template <typename PutcFunc>
int print_string(PutcFunc putc_f, const char* str) {
    int ret = 0;
    while(*str) {
        putc_f(*(str++));
        ret ++;
    }
    return ret;
}

inline bool is_numeric_format_type_specifier(char c) {
    return c == 'd' || c == 'x' || c == 'i';
}

inline bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

#define MAX_FILL_NUMBER_LEN 16
/**
 * returns fill data (minimal number of char, fill char) for a specific format specifier in a format string
 * fmt should point to the format type specifier
 * for example, if called for the format specifier '%3d' in the format string 'the number is: %3d',
 * then fmt should point to 'd'
 */ 
static int get_fill_data(const char* fmt, int& min_chars, char& fill_char) {
    ASSERT(is_numeric_format_type_specifier(*fmt));

    char fill_number[MAX_FILL_NUMBER_LEN];
    const char* fmt_start = fmt;
    // find '%'
    while(*fmt_start != '%') {
        fmt_start --;
    }
    fmt_start++;
    // if not fill info, e.g just a simple '%d'
    if(!is_digit(*fmt_start)) {
        min_chars = 0;
        fill_char = 0;
        return 0;
    }
    // we fill with 0s if number starts with '0' (e.g '%05d'),
    // otherwise we fill with ' ' (e.g '%5d')
    if(*fmt_start == '0') { // zero fill
        fill_char = '0';
        fmt_start++;
    } else{
        fill_char = ' ';
    }
    int i = 0;
    for(; is_digit(fmt_start[i]); i++) {
        if(i >= MAX_FILL_NUMBER_LEN - 2) { // leave space for null terminator
            // FIXME: set errno
            return 1;
        }
        fill_number[i] = fmt_start[i];
    }
    fill_number[i] = 0;
    min_chars = df_atoi(fill_number, 10);
    return 0;
}


template <typename PutcFunc>
int printf_internal(PutcFunc putc_f, const char* fmt, va_list args) {
    int ret = 0;

    for(;*fmt != '\0'; fmt++) {
        if(*fmt == '%' && *(fmt+1)) {
            fmt++;
            int min_chars = 0;
            char fill_char = 0;
            while(*fmt >= '0' && *fmt <= '9') { // skip digits
                fmt++;
            }
            switch(*fmt) {
                case 'i':
                    [[fallthrough]];
                case 'd':
                    ASSERT(!get_fill_data(fmt, min_chars, fill_char));
                    ret += print_int_dec(putc_f, va_arg(args, int), min_chars, fill_char);
                    break;
                case 'x':
                    ASSERT(!get_fill_data(fmt, min_chars, fill_char));
                    ret += print_int_hex(putc_f, va_arg(args, uint32_t), min_chars, fill_char);
                    break;
                case 'p':
                    ret += print_string(putc_f, "0x");
                    ret += print_int_hex(putc_f, va_arg(args, uint32_t), min_chars, fill_char);
                    break;
                case 's':
                    ret += print_string(putc_f, va_arg(args, char*));
                    break;
                case 'c':
                    putc_f((char)va_arg(args, int));
                    ret += 1;
                    break;
                case '%':
                    putc_f('%');
                    ret++;
                    break;
                default:
                    ASSERT(false);
                    return -1;
            }
        } else {
            putc_f(*fmt);
            ret ++;
        }
    }
    return ret;
}

template <typename PutcFunc>
int printf_internal_wrapper(PutcFunc putc_f, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const int rc = printf_internal(putc_f, fmt, args);
    va_end(args);
    return rc;
}
