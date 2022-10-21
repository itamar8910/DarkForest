
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

    static void send_ping(IPV4 destination, uint16_t id, uint16_t sequence_number);

    static void fix_checksum_and_send(IPV4 destination, const u8* buffer, size_t size);
};

}
