
#include "unistd.h"
#include "asserts.h"
#include "stdio.h"
#include "constants.h"

void print_file(const String& path) {
    int fd = std::open(path.c_str());
    if(fd < 0) {
        printf("error opening file: %s\n", path.c_str());
        return;
    }
    int size = std::file_size(fd);
    if(size < 0) {
        printf("error getting file size: %s\n", path.c_str());
        return;
    }
    Vector<char> buff(size+1);
    int rc = std::read(fd, buff.data(), size);
    if(rc != size) {
        printf("error reading file: %s\n", path.c_str());
        return;
    }
    buff.data()[size] = 0;
    printf("%s\n", buff.data());
}

int main(char** argv, size_t argc) {
    if(argc < 1) {
        printf("expected argc >= 1\n");
        return 1;
    }
    if(argc == 1) {
        printf("Usage: %s [file1] [file2] ...\n", argv[0]);
        return 1;
    }
    for(size_t i = 1; i < argc; i++) {
        print_file(argv[i]);
    }
    return 0;
}