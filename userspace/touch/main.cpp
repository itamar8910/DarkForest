
#include "df_unistd.h"
#include "asserts.h"
#include "stdio.h"
#include "constants.h"
#include "kernel/errs.h"

void do_touch(const String& path)
{
    int rc = std::create_file(path);
    if(rc < 0)
    {
        printf("error when creating file: %d\n", rc);
    }
}

int main(char** argv, size_t argc) {
    if(argc < 1)
    {
        printf("expected argc >= 1\n");
        return 1;
    }
    if(argc != 2) {
        printf("usage: $s [path]\n", argv[0]);
        return 1;
    }

    String path = argv[1];

    do_touch(path);

    printf("\n");
    return 0;
}
