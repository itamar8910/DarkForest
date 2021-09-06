#include "stat.h"
#include "libc/asserts.h"
#include "string.h"
#include "df_unistd.h"

extern "C" {

int mkdir(const char * path, mode_t )
{
    if (strcmp(path, ".") == 0) {
        return 0;
    }
    int rc =  std::create_directory(path);
    kprintf("*** mkdir: %s, rc=%d\n", path, rc);
    return rc;
}

}
