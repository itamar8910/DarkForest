#include "unistd.h"

int func() {
    asm volatile(
        // "1:\n"
        "mov $3, %eax\n"
        "int $0x80\n"
        // "jmp 1b\n"
    );
    return 4;
}

int main() {
    int x = func();
    exit(x);
    return 0;
}