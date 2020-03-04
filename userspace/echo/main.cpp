
#include "unistd.h"
#include "asserts.h"
#include "stdio.h"
#include "constants.h"
#include "kernel/errs.h"

void do_echo(const String& path, const String& text) {
    int fd = std::open(path.c_str());
    if(fd < 0) {
        printf("error opening file: %s\n", path.c_str());
        return;
    }
    int rc = std::write(fd, text.c_str(), text.len());
    if(rc < 0)
    {
        printf("error in write: %d\n", rc);
    }
}

int main(char** argv, size_t argc) {
    if(argc < 1)
    {
        printf("expected argc >= 1\n");
        return 1;
    }
    if(argc != 4) {
        printf("usage: $s [text] > [path to file]\n", argv[0]);
        return 1;
    }

    String text = String(argv[1]) + "\n";
    String path = argv[3];

    do_echo(path, text);

    printf("\n");
    return 0;
}