#include "list.h"
#include "Kassert.h"


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
    kprintf("test iterartion\n");
    List<B*> l;
    l.append(new B(1));
    l.append(new B(2));
    l.append(new B(3));
    for(auto* item = l.head(); item != nullptr; item = item->next) {
        kprintf("%d,", item->val->x);
    }
    kprintf("\n");
}

static void list_tests() {
    kprintf("[list_tests]\n");
    List<int> l;
    ASSERT(l.size() == 0, "0");
    l.append(5);
    ASSERT(l.size() == 1, "1");
    l.append(8);
    ASSERT(l.size() == 2, "2");
    ASSERT(!l.remove(9), "2a");
    ASSERT(l.remove(8), "2b");
    ASSERT(l.size() == 1, "3");
    l.append(2);
    ASSERT(l.size() == 2, "4");
    l.remove(5);
    ASSERT(l.size() == 1, "5");
    l.append(3);
    ASSERT(l.size() == 2, "6");
    ASSERT(l.remove(2), "6a");
    ASSERT(l.remove(3), "6b");
    ASSERT(l.size() == 0, "7");
    l.append(4);
    l.append(5);
    l.append(11);
    ASSERT(l.size() == 3, "8");

    // test that remove an item* from list does not delete item itself
    List<A*> l2;
    ASSERT(glob_A_ctor_called == 0, "9");
    auto* a_ptr = new A();
    ASSERT(a_ptr->alive, "10");
    l2.append(a_ptr);
    ASSERT(glob_A_ctor_called == 1, "9a");
    ASSERT(l2.remove(a_ptr), "10");
    ASSERT(a_ptr->alive, "10a");
    ASSERT(glob_A_ctor_called == 1, "10b");

    delete a_ptr;

    test_iteration();

}