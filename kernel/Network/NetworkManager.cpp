#include "NetworkManager.h"
#include "Ethernet.h"
#include "Arp.h"
#include "drivers/RTL8139NetworkCard.h"
#include "Scheduler.h"
#include "IpV4.h"

// #define NETWORK_DBG

namespace Network
{

NetworkManager::NetworkManager(MAC our_mac) :
    m_our_mac(our_mac),
    m_our_ip({192,168,2,20}),
    m_gateway_ip({192,168,2,1}),
    m_rx_lock("NetworkRxLock")
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
    // TODO: This is called from interrupt handler. We should only queue this packet.
    LOCKER(m_rx_lock);
    m_rx_queue.append(Vector<u8>(packet, size));
}

struct ArpCacheEntry
{
    IPV4 ip;
    MAC mac;
};

static Vector<ArpCacheEntry>* arp_cache = nullptr;

bool NetworkManager::resolve_arp(IPV4 ip, MAC& out_mac)
{
    if (!arp_cache)
    {
        arp_cache = new Vector<ArpCacheEntry>();
    }

    for (auto& entry : *arp_cache)
    {
        if (entry.ip == ip)
        {
            out_mac = entry.mac;
            return true;
        }
    }

    bool result = Arp::the().send_arp_request(ip, our_ip(), out_mac);
    if (result)
        arp_cache->append({ip, out_mac});

    return result;
}


void NetworkManager::handle_received_packets_task_static()
{
    NetworkManager::the().handle_received_packets_task();
}

void NetworkManager::handle_received_packets_task()
{
    for(;;)
    {
        Scheduler::the().block_current(&m_pending_rx_blocker);
        {
            LOCKER(m_rx_lock);
            ASSERT(!m_rx_queue.empty());
            auto packet = m_rx_queue.pop_front();
            handle_packet(packet.data(), packet.size());
        }
    }
}

void NetworkManager::handle_packet(u8* packet, size_t size)
{
    if (size < sizeof(Network::Ethernet::EthernetHeader))
    {
        kprintf("on_packet_received: packet too small\n");
        return;
    }

    Ethernet::EthernetHeader* const ethernet_header = reinterpret_cast<Network::Ethernet::EthernetHeader*>(packet);
    ethernet_header->flip_endianness();

    static constexpr MAC BROADCAST_MAC = {0xff,0xff,0xff,0xff,0xff,0xff};

    if (ethernet_header->destination != m_our_mac && ethernet_header->destination != BROADCAST_MAC)
    {
#ifdef NETWORK_DBG
        kprintf("Received ethernet packet with unknown destination\n");
#endif
        return;
    }

    u8* data_after_ether_header = packet + sizeof(Network::Ethernet::EthernetHeader);
    size_t size_after_ether_header = size - sizeof(Network::Ethernet::EthernetHeader);

    switch (ethernet_header->ethertype)
    {
        case Ethernet::EtherType::ARP:
            Arp::the().on_arp_message_received(data_after_ether_header, size_after_ether_header);
            break;
        case Ethernet::EtherType::IPV4:
            IpV4::on_packet_received(data_after_ether_header, size_after_ether_header);
        default:
#ifdef NETWORK_DBG
            kprintf("Ethernet packet with unknown EtherType: %x\n", ethernet_header->ethertype);
#endif
            break;
    }
}

bool NetworkManager::PendingRxBlocker::can_unblock()
{
    return !NetworkManager::the().m_rx_queue.empty();
}

}