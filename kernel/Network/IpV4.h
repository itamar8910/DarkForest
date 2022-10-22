#pragma once

#include "types.h"
#include "constants.h"
#include "NetworkTypes.h"

namespace Network
{

class IpV4 final
{
public:
    IpV4() = delete;

    enum class Protocol : uint8_t
    {
        ICMP = 0x1,
    };

    static void send(IPV4 destination, Protocol protocol, const u8* payload, size_t payload_size);
    static void on_packet_received(u8* packet, size_t size);

};

}
