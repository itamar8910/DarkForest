#include "unistd.h"
#include "stdio.h"
#include "asserts.h"
#include "mman.h"
#include "string.h"
#include "malloc.h"

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

    // void* addr = (void*) 0x50000000;
    // allocate_page(addr); 
    char* str = new char[100]; 
    strcpy(str, "This is in a user allocated page!\n");
    puts(str);
    return 0;
    // int x = func();
    // return x;
}