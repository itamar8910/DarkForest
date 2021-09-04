#include "strings.h"
#include "asserts.h"

extern "C"{

int strcasecmp(const char *, const char *)
{
    ASSERT_NOT_REACHED();
}

int strncasecmp(const char *, const char *, size_t )
{

    ASSERT_NOT_REACHED();
}

}
