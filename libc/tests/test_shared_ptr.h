#include "shared_ptr.h"
#include "logging.h"

static int ctor_counter = 0;
static int dtor_counter = 0;

class AA{
public:
    AA(){ctor_counter++;}
    ~AA(){dtor_counter++;}
};

void test_shared_ptr() {
    kprintf("[shared_ptr tests]\n");
    ASSERT(ctor_counter==0);
    ASSERT(dtor_counter==0);

    {
        shared_ptr<AA> p(new AA());
        ASSERT(p.num_refs()==1);
        ASSERT(ctor_counter==1);
        ASSERT(dtor_counter==0);
        {
            shared_ptr<AA>p2 = p;
            ASSERT(p.num_refs()==2);
            ASSERT(p2.num_refs()==2);
            ASSERT(p.get()==p2.get());
            ASSERT(ctor_counter==1);
            ASSERT(dtor_counter==0);
        }
    }
    ASSERT(ctor_counter==1);
    ASSERT(dtor_counter==1);
}
