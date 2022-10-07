#include "NetworkManager.h"
#include "Ethernet.h"
#include "Arp.h"
#include "drivers/RTL8139NetworkCard.h"

// #define NETWORK_DBG

namespace Network
{

NetworkManager::NetworkManager(MAC our_mac) :
    m_our_mac(our_mac),
    m_our_ip({192,168,2,20}),
    m_gateway_ip({192,168,2,1})
{
}

NetworkManager* instance = nullptr;

void NetworkManager::initialize(MAC our_mac)
{
    instance = new NetworkManager(our_mac);
}

NetworkManager& NetworkManager::the()
{
    ASSERT(instance != nullptr);
    return *instance;
}

void NetworkManager::on_packet_received(u8* packet, size_t size)
{
    if (size < sizeof(Network::Ethernet::EthernetHeader))
    {
        kprintf("on_packet_received: packet too small\n");
        return;
    }

    // TODO: Check mac

    Ethernet::EthernetHeader* const ethernet_header = reinterpret_cast<Network::Ethernet::EthernetHeader*>(packet);
    ethernet_header->flip_endianness();

    if (ethernet_header->destination != m_our_mac)
    {
#ifdef NETWORK_DBG
        kprintf("Received ethernet packet with unknown destination\n");
#endif
        return;
    }

    switch (ethernet_header->ethertype)
    {
        case Ethernet::EtherType::ARP:
            Arp::the().on_arp_message_received(packet + sizeof(Network::Ethernet::EthernetHeader), size - sizeof(Network::Ethernet::EthernetHeader));
            break;
        default:
#ifdef NETWORK_DBG
            kprintf("Ethernet packet with unknown EtherType: %x\n", ethernet_header->ethertype);
#endif
            break;
    }
}

}