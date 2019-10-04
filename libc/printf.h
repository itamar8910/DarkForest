#pragma once

#include "stdarg.h"

// template <typename PutcFunc>
// void print_int_dec(PutcFunc putc_f, int val);

// template <typename PutcFunc>
// void printf_internal(PutcFunc putc_f, const char* fmt, va_list args);

const char digits_dec[] = "0123456789";
const char digits_hex[] = "0123456789abcdef";

template <int Base, typename PutcFunc>
void print_int(PutcFunc putc_f, const char digits[], int val) {
    if(val == 0) {
        putc_f('0');
        return;
    }
    int sign_pos = (val > 0); // sign positive flag
    if(!sign_pos) {
        val *= -1;
    }
    int n_digits = 0;
    int temp = val;
    int pow = 1;
    while(temp > 0) {
        n_digits += 1;
        pow *= Base;
        temp /= Base;
    }
    pow /= Base;
    if(!sign_pos) {
        putc_f('-');
    }
    for(int i = n_digits - 1; i >= 0; i--, pow /= 10) {
        uint8_t dig = (val / pow);
        putc_f(digits[dig]);
        val = val % pow;
    }
}

template <typename PutcFunc>
void print_int_dec(PutcFunc putc_f, int val) {
    print_int<10>(putc_f, digits_dec, val);
}

template <typename PutcFunc>
void print_int_hex(PutcFunc putc_f, int val) {
    print_int<16>(putc_f, digits_hex, val);
}

template <typename PutcFunc>
void print_string(PutcFunc putc_f, char* str) {
    while(*str) {
        putc_f(*(str++));
    }
}

template <typename PutcFunc>
void printf_internal(PutcFunc putc_f, const char* fmt, va_list args) {
    int intval = 0;
    char* strval = 0;
    while(*fmt != '\0') {
        if(*fmt == '%') {
            switch(fmt[1]) {
                case 'd':
                    intval = va_arg(args, int);
                    print_int_dec(putc_f, intval);
                    fmt+=2;
                    break;
                case 'x':
                    intval = va_arg(args, int);
                    print_int_hex(putc_f, intval);
                    fmt+=2;
                    break;
                case 's':
                    strval = va_arg(args, char*);
                    // print_int_dec(putc_f, strval[0]);
                    print_string(putc_f, strval);
                    fmt += 2;

            }

        } else {
            putc_f(*fmt);
            fmt++;
        }
    }
}