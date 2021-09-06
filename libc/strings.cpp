#include "strings.h"
#include "asserts.h"

extern "C"{


static int isalpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static char tolower(char c) {
    if (c >= 'A' && c <= 'Z')
        return c + ('a'-'A');
    return c;
}

/// Taken from SerenityOS's LibC

static char foldcase(char ch)
{
    if (isalpha(ch))
        return tolower(ch);
    return ch;
}

int strcasecmp(const char* s1, const char* s2)
{
    for (; foldcase(*s1) == foldcase(*s2); ++s1, ++s2) {
        if (*s1 == 0)
            return 0;
    }
    return foldcase(*(const unsigned char*)s1) < foldcase(*(const unsigned char*)s2) ? -1 : 1;
}

///

int strncasecmp(const char *, const char *, size_t )
{

    ASSERT_NOT_REACHED();
}

}
