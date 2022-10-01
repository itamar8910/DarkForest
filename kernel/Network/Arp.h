#pragma once

#include "types.h"
#include "constants.h"
#include "NetworkTypes.h"

namespace Network
{
    class Arp final
    {
    public:
        Arp() = delete;

        static void send_arp_request(const IPV4 target_ip, const IPV4 sender_ip);
    };
}