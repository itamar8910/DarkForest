#include "NetworkManager.h"
#include "Ethernet.h"
#include "Arp.h"
#include "drivers/RTL8139NetworkCard.h"

namespace Network
{

NetworkManager::NetworkManager(MAC our_mac) :
    m_our_mac(our_mac)
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
        kprintf("Received ethernet packet with unknown destination\n");
        return;
    }

    switch (ethernet_header->ethertype)
    {
        case Ethernet::EtherType::ARP:
            Arp::the().on_arp_message_received(packet + sizeof(Network::Ethernet::EthernetHeader), size - sizeof(Network::Ethernet::EthernetHeader));
            break;
        default:
            kprintf("Ethernet packet with unknown EtherType: %x\n", ethernet_header->ethertype);
            break;
    }
}

}