
#pragma once

#include "types.h"
#include "constants.h"
#include "NetworkTypes.h"
#include "IcmpSocket.h"
#include "types/list.h"
#include "lock.h"

namespace Network
{

class Icmp final
{
public:
    static Icmp& the();

    void send_ping(IPV4 destination, uint16_t id, uint16_t sequence_number);
    void fix_checksum_and_send(IPV4 destination, const u8* buffer, size_t size);
    void on_packet_received(u8* packet, size_t size, IPV4 source);
    void register_socket(IcmpSocket& socket);
    void unregister_socket(IcmpSocket& socket);

private:
    Icmp() = default;

    List<IcmpSocket*> m_sockets;
    Lock m_lock {"Icmp"};
};

}
