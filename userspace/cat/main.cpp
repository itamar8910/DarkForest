
#include "unistd.h"
#include "asserts.h"
#include "stdio.h"
#include "constants.h"

#define USERSPACE_STACK 0xb0000000

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
   u32 user_args_end = USERSPACE_STACK + PAGE_SIZE*2;
   u32 user_args_start = user_args_end - PAGE_SIZE;
   char*** argv_ptr = (char***) (user_args_start+4);
   size_t* argc_ptr = (size_t*) (user_args_start+8);
   char** argv = *argv_ptr;
   size_t argc = *argc_ptr;
   kprintf("argc: %d\n", argc);
   kprintf("argv[0]: %s\n", argv[0]);
    return 0;
}