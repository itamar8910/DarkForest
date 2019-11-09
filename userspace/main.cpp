#include "unistd.h"
#include "stdio.h"

int func() {
    for(;;) {
        puts("hello from usermode\n");
        sleep_ms(1000);
    }
    return 0;
}

int main() {
    int x = func();
    return x;
}