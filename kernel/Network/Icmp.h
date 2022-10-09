
#pragma once

#include "types.h"
#include "constants.h"
#include "NetworkTypes.h"

namespace Network
{

class Icmp final
{
public:
    Icmp() = delete;

    enum class Type : uint8_t
    {
        Request=8,
        Reply=0, 
    };

    static void send_ping(IPV4 destination, uint16_t id, uint16_t sequence_number);

};

}
