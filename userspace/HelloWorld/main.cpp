#include "unistd.h"
#include "stdio.h"
#include "asserts.h"

int main() {
    printf("hello, world!\n");
    // generate a segfault
    char* p = nullptr;
    *p = 1;
    return 0;
}