#pragma once

#include "types.h"
#include "constants.h"
#include "NetworkTypes.h"

namespace Network
{
class Arp final
{
public:

    static Arp& the();

    static void send_arp_request(const IPV4 target_ip, const IPV4 sender_ip);

    void on_arp_message_received(const u8* message, size_t size);

private:
    Arp() = default;
};
}