#pragma once

#include "stdarg.h"

// template <typename PutcFunc>
// void print_int_dec(PutcFunc putc_f, int val);

// template <typename PutcFunc>
// void printf_internal(PutcFunc putc_f, const char* fmt, va_list args);

template <typename PutcFunc>
void print_int_dec(PutcFunc putc_f, int val) {
    if(val == 0) {
        putc_f('0');
        return;
    }
    int sign_pos = (val > 0);
    if(!sign_pos) {
        val *= -1;
    }
    int n_digits = 0;
    int temp = val;
    int pow = 1;
    while(temp > 0) {
        n_digits += 1;
        pow *= 10;
        temp /= 10;
    }
    pow /= 10;
    if(!sign_pos) {
        putc_f('-');
    }
    for(int i = n_digits - 1; i >= 0; i--, pow /= 10) {
        uint8_t dig = (val / pow);
        putc_f('0' + (char)dig);
        val = val % pow;
    }
}

template <typename PutcFunc>
void printf_internal(PutcFunc putc_f, const char* fmt, va_list args) {
    while(*fmt != '\0') {
        if(*fmt == '%') {
            switch(fmt[1]) {
                case 'd':
                    int val = va_arg(args, int);
                    print_int_dec(putc_f, val);
                    fmt+=2;
                    break;

            }

        } else {
            putc_f(*fmt);
            fmt++;
        }
    }
}