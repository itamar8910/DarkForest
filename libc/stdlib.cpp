#include "stdlib.h"

int atoi(const char* str) {
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