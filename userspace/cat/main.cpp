
#include "unistd.h"
#include "asserts.h"
#include "stdio.h"
#include "constants.h"

void print_hello_text() {
    int fd = std::open("/initrd/a.txt");
    ASSERT(fd>=0);
    int size = std::file_size(fd);
    ASSERT(size > 0);
    char* buff = new char[size+1];
    int rc = std::read(fd, buff, size);
    ASSERT(rc == size);
    buff[size] = 0;
    printf("%s\n", buff);
}

int main(char** argv, size_t argc) {
    print_hello_text();
    kprintf("argc: %d\n", argc);
    if(argc > 0)
        kprintf("argv[0]: %s\n", argv[0]);
    return 0;
}