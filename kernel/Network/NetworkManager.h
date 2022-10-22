#pragma once

#include "types.h"
#include "NetworkTypes.h"
#include "lock.h"
#include "TaskBlocker.h"
#include "types/list.h"

namespace Network
{

class NetworkManager final
{
public:

    static void initialize(MAC our_mac); 
    static NetworkManager& the();

    static void handle_received_packets_task_static();

    void on_packet_received(u8* packet, size_t size);
    void transmit(const u8* packet, size_t size);

    IPV4 our_ip() const { return m_our_ip; }
    MAC our_mac() const { return m_our_mac; }
    IPV4 gateway_ip() const { return m_gateway_ip; }

    bool resolve_arp(IPV4 ip, MAC& out_mac);

private:
    class PendingRxBlocker : public TaskBlocker {
    public:
        PendingRxBlocker() = default;
        virtual bool can_unblock() override;
        ~PendingRxBlocker() override = default;
    };

    NetworkManager(MAC our_mac);
    void handle_received_packets_task();
    void handle_packet(u8* packet, size_t size);

    MAC m_our_mac {};
    IPV4 m_our_ip {};
    IPV4 m_gateway_ip {};

    PendingRxBlocker m_pending_rx_blocker;
    List<Vector<u8>> m_rx_queue;
    Lock m_rx_lock;
};

}
