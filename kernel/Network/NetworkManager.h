#pragma once

#include "types.h"
#include "Network/NetworkTypes.h"

namespace Network
{

class NetworkManager final
{
public:

    static void initialize(MAC our_mac); 
    static NetworkManager& the();

    void on_packet_received(u8* packet, size_t size);
    void transmit(const u8* packet, size_t size);

    IPV4 our_ip() const { return m_our_ip; }
    IPV4 gateway_ip() const { return m_gateway_ip; }

private:
    NetworkManager(MAC our_mac);

    MAC m_our_mac {};
    IPV4 m_our_ip {};
    IPV4 m_gateway_ip {};
};

}
