
#include "asserts.h"


extern "C" void __cxa_pure_virtual()
{
    ASSERT_NOT_REACHED("A pure virtual function has been called");
}