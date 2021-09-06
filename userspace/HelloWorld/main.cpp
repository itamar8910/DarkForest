#include "df_unistd.h"
#include "stdio.h"
#include "asserts.h"
#include "unistd.h"
#include "stdio.h"

#ifdef KERNEL
static_assert(0);
#endif


int main() {
    kprintf("hello, world!\n");

    FILE* f = fopen("/stuff/a.txt", "r");
    int savedpos;
    int length;

    // save the current position in the file
    savedpos = ftell(f);
    
    // jump to the end and find the length
    fseek(f, 0, SEEK_END);
    length = ftell(f);

    // go back to the old location
    fseek(f, savedpos, SEEK_SET);

    printf("size: %d\n", length);


    // generate a segfault
    // char* p = nullptr;
    // *p = 1;
    return 0;
}
