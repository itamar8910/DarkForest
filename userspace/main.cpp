#include "unistd.h"
#include "stdio.h"
#include "asserts.h"

int func() {
    for(;;) {
        puts("hello from usermode\n");
        printf("abcde this?\n");
        printf("the number one is: %d\n", 1);

        sleep_ms(1000);
    }
    return 0;
}

int main() {
    int x = func();
    return x;
}