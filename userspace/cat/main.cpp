
#include "unistd.h"
#include "asserts.h"
#include "stdio.h"


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

int main() {
    // std::sleep_ms(500);
    print_hello_text();
    return 0;
}