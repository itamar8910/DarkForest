#include "NetworkTypes.h"
#include "stdio_shared.h"

namespace Network
{

String MAC::to_string() const
{
    static constexpr size_t BUFFLEN = 20;
    char buff[BUFFLEN] {};
    size_t current_length = 0;
    for (size_t i =0 ; i < MAC_SIZE; ++i)
    {
        current_length += snprintf(buff+current_length, BUFFLEN - current_length, "%x", data[i]);
        if (i != MAC_SIZE - 1)
        {
            buff[current_length] = ':';
            ++current_length;
        }
    }
    return String(buff, strlen(buff));
}
}