#include "stat.h"
#include "libc/asserts.h"
#include "string.h"

extern "C" {

int mkdir(const char * path, mode_t )
{
    if (strcmp(path, ".") == 0) {
        return 0;
    }
    ASSERT_NOT_REACHED();
}

}
