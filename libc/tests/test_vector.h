
#include "logging.h"
#include "asserts.h"
#include "types/vector.h"

static void vector_tests1() {
    Vector<int> v;
    ASSERT(v.size() == 0);
    v.append(2);
    ASSERT(v[0] == 2);
    v.append(3);
    ASSERT(v.size() == 2);
    Vector<int> v2 = Vector<int>();
    for(int i = 0; i < 100 ; i++) {
        v2.append(i);
        ASSERT(v2[i] == i);
    }
    ASSERT(v2.size() == 100);
    for(int i = 0; i < 100 ; i++) {
        ASSERT(v2[i] == i);
    }
    // test vector assignment operator
    // (ensure we don't do a double free)
    v2 = v;
}

static void vector_tests2() {
    Vector<int> v1;
    v1.append(8);
    Vector<int> v2 = v1;
    ASSERT(v1[0] == 8);
    ASSERT(v2[0] == 8);
    v1[0] = 9;
    ASSERT(v1[0] == 9);
    ASSERT(v2[0] == 8);
    v1 = v2;
    ASSERT(v1[0] == 8);
    v2[0] = 3;
    ASSERT(v1[0] == 8);
    ASSERT(v2[0] == 3);
}

static void vector_tests3() {
    Vector<int> v;
    v.append(1);
    v.append(4);
    v.append(2);
    u32 sum = 0;
    for(auto& x : v) {
        sum += x;
    }
    ASSERT(sum == 7);
}

static void vector_tests() {
    kprintf("[vector_tests]\n");
    vector_tests1();
    vector_tests2();
    vector_tests3();

}