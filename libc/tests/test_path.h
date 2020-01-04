
#include "FileSystem/path.h"
#include "asserts.h"


static void path_tests()
{
    kprintf("[path_tests]\n");
    auto path = Path("/home/user/Documents/hello.txt");
    ASSERT(path.type() == Path::PathType::Absolute);
    ASSERT(path.num_parts() == 4);
    ASSERT(path.get_part(0) == "home");
    ASSERT(path.get_part(1) == "user");
    ASSERT(path.get_part(2) == "Documents");
    ASSERT(path.get_part(3) == "hello.txt");
}