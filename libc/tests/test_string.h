#include "types/String.h"
#include "Kassert.h"
#include "logging.h"

void string_tests() {
    kprintf("[string_test]\n");
    String s1 = String("hello");
    String s2 = String("hello");
    ASSERT(s1==s2, "1");
    s2 = String("world");
    ASSERT(s1!=s2, "2");
    // kprintf("%s %s\n", s1.c_str(), s2.c_str());
    ASSERT(strlen(s1.c_str())==s1.len(), "3");
    s1 = "123";
    ASSERT(s1 == String("123"), "4");


}