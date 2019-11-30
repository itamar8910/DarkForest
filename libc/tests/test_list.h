#include "types/list.h"
#include "asserts.h"


int glob_A_ctor_called = 0;

struct A {
    bool alive;
    A(): alive(true){glob_A_ctor_called++;}
    ~A() {alive=false;}
};

struct B{
    int x;
    B(int _x): x(_x){}
};

static void test_iteration() {
    List<B*> l;
    l.append(new B(1));
    l.append(new B(2));
    l.append(new B(3));
    size_t c1 = 0, c2 = 0;
    for(auto* item = l.head(); item != nullptr; item = item->next) {
        (void)item;
        ++c1;
    }
    for(auto& item : l) {
        (void)item;
        ++c2;
    }
    ASSERT(l.size()==3);
    ASSERT(c1==3);
    ASSERT(c2==3);
}

static void list_tests() {
    kprintf("[list_tests]\n");
    List<int> l;
    ASSERT(l.size() == 0);
    l.append(5);
    ASSERT(l.size() == 1);
    l.append(8);
    ASSERT(l.size() == 2);
    ASSERT(!l.remove(9));
    ASSERT(l.remove(8));
    ASSERT(l.size() == 1);
    l.append(2);
    ASSERT(l.size() == 2);
    l.remove(5);
    ASSERT(l.size() == 1);
    l.append(3);
    ASSERT(l.size() == 2);
    ASSERT(l.remove(2));
    ASSERT(l.remove(3));
    ASSERT(l.size() == 0);
    l.append(4);
    l.append(5);
    l.append(11);
    ASSERT(l.size() == 3);

    // test that remove an item* from list does not delete item itself
    List<A*> l2;
    ASSERT(glob_A_ctor_called == 0);
    auto* a_ptr = new A();
    ASSERT(a_ptr->alive);
    l2.append(a_ptr);
    ASSERT(glob_A_ctor_called == 1);
    ASSERT(l2.remove(a_ptr));
    ASSERT(a_ptr->alive);
    ASSERT(glob_A_ctor_called == 1);

    delete a_ptr;

    test_iteration();

}