#include "math.h"
#include "asserts.h"

extern "C" {
double fabs(double)
{
    ASSERT_NOT_REACHED();
}
}
