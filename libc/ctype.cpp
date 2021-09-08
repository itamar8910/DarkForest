#include "ctype.h"
#include "asserts.h"

extern "C" {
int toupper(int c)
{
    if (c >= 'a' && c <= 'z')
        return c - ('a'-'A');
    return c;
}

int tolower(int c) {
    if (c >= 'A' && c <= 'Z')
        return c + ('a'-'A');
    return c;
}

}
