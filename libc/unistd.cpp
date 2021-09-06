#include "unistd.h"
#include "df_unistd.h"

extern "C" {

void exit(int status)
{
    std::exit(status);
}

}
