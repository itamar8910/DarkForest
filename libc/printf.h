#pragma once

#include "stdarg.h"
#include "DebugPort.h"
// template <typename PutcFunc>
// void print_int_dec(PutcFunc putc_f, int val);

// template <typename PutcFunc>
// void printf_internal(PutcFunc putc_f, const char* fmt, va_list args);

const char digits_dec[] = "0123456789";
const char digits_hex[] = "0123456789abcdef";

template <int Base, typename PutcFunc>
int print_int(PutcFunc putc_f, const char digits[], uint32_t val, int is_signed=false) {
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
    for(int i = n_digits - 1; i >= 0; i--, pow /= Base) {
        uint8_t dig = (val / pow);
        putc_f(digits[dig]);
        ret++;
        val = val % pow;
    }
    return ret;
}

template <typename PutcFunc>
int print_int_dec(PutcFunc putc_f, int val) {
    return print_int<10>(putc_f, digits_dec, val, true);
}

template <typename PutcFunc>
int print_int_hex(PutcFunc putc_f, uint32_t val) {
    return print_int<16>(putc_f, digits_hex, val);
}

template <typename PutcFunc>
int print_string(PutcFunc putc_f, char* str) {
    int ret = 0;
    while(*str) {
        putc_f(*(str++));
        ret ++;
    }
    return ret;
}

template <typename PutcFunc>
int printf_internal(PutcFunc putc_f, const char* fmt, va_list args) {
    int ret = 0;
    for(;*fmt != '\0'; fmt++) {
        if(*fmt == '%' && *(fmt+1)) {
            fmt++;
            switch(*fmt) {
                case 'd':
                    ret += print_int_dec(putc_f, va_arg(args, int));
                    break;
                case 'x':
                    ret += print_int_hex(putc_f, va_arg(args, uint32_t));
                    break;
                case 's':
                    ret += print_string(putc_f, va_arg(args, char*));
                    break;
                case '%':
                    putc_f('%');
                    ret++;
                    break;
                default:
                    // TODO: kprintf("bad format specifier") 
                    return -1;
            }
        } else {
            putc_f(*fmt);
            ret ++;
        }
    }
    return ret;
}