#include "Arp.h"
#include "drivers/RTL8139NetworkCard.h"
#include "cstring.h"
#include "Ethernet.h"
#include "bits.h"
#include "Scheduler.h"
#include "sleep.h"

namespace Network
{

Arp* s_arp = nullptr;
Arp& Arp::the()
{
    if (!s_arp)
    {
        s_arp = new Arp();
    }
    return *s_arp;
}

struct __attribute__((__packed__)) ArpMessage
{
    enum : uint16_t
    {
        Ethernet = 0x1,
    } hardware_type;

    enum : uint16_t
    {
        Ipv4 = 0x0800,
    } protocol_type;

    uint8_t hardware_size;
    uint8_t protocol_size;

    enum : uint16_t
    {
        Request = 0x1,
        Response = 0x2
    } opcode;

    MAC sender_mac;
    IPV4 sender_ip_address;
    MAC target_mac;
    IPV4 target_ip_address;

    void flip_endianness();
};

void ArpMessage::flip_endianness()
{
    hardware_type = (__typeof__(hardware_type)) to_flipped_endianness((uint16_t)hardware_type);
    protocol_type = (__typeof__(protocol_type)) to_flipped_endianness((uint16_t)protocol_type);
    opcode = (__typeof__(opcode)) to_flipped_endianness((uint16_t)opcode);
}

bool Arp::send_arp_request(const IPV4 target_ip, const IPV4 sender_ip, MAC& out_answer)
{
    auto arp_data = ArpMessage{
        .hardware_type = ArpMessage::Ethernet,
        .protocol_type = ArpMessage::Ipv4,
        .hardware_size = 6,
        .protocol_size = 4,
        .opcode = ArpMessage::Request,
        .sender_mac = {},
        .sender_ip_address = {},
        .target_mac = {},
        .target_ip_address = {},
    };

    static constexpr MAC BROADCAST_MAC = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    memcpy(arp_data.sender_mac.data, RTL8139NetworkCard::the().mac().data, MAC_SIZE);
    memcpy(arp_data.sender_ip_address.data, sender_ip.data, IPV4_SIZE);
    memcpy(arp_data.target_mac.data, BROADCAST_MAC.data, MAC_SIZE);
    memcpy(arp_data.target_ip_address.data, target_ip.data, IPV4_SIZE);

    arp_data.flip_endianness();

    auto ether = Ethernet::build(arp_data.target_mac, arp_data.sender_mac, Ethernet::EtherType::ARP, (u8*)&arp_data, sizeof(arp_data));

    static constexpr u32 TIMEOUT_MS = 5000;
    shared_ptr<PendingRequestBlocker> blocker (new PendingRequestBlocker(target_ip, TIMEOUT_MS));
    m_arp_blockers.append(blocker);

    RTL8139NetworkCard::the().transmit(ether->data(), ether->size());

    kprintf("blocking until arp response\n");


    Scheduler::the().block_current(blocker.get());

    if (!blocker->has_answer())
    {
        kprintf("ARP request timed-out\n");
        return false;
    }
    kprintf("got arp answer\n");
    out_answer = blocker->answer();
    return true;
}

void Arp::on_arp_message_received(u8* message, size_t size)
{
    (void)message;
    kprintf("arp_message_received: %d\n", size);
    if (size < sizeof(ArpMessage))
    {
        kprintf("ARP message too small\n");
        return;
    }
    ArpMessage* arp_message = reinterpret_cast<ArpMessage*>(message);
    arp_message->flip_endianness();

    if (arp_message->opcode != ArpMessage::Response)
    {
        return;
    }

    for (auto& blocker : m_arp_blockers)
    {
        if (blocker->target() == arp_message->sender_ip_address)
        {
            blocker->set_answer(arp_message->sender_mac);
        }
    }
}

Arp::PendingRequestBlocker::PendingRequestBlocker(IPV4 target, u32 timeout_ms) :
    m_target(target),
    m_timeout_timestamp(time_since_boot_ms() + timeout_ms)
{
}

bool Arp::PendingRequestBlocker::can_unblock()
{
    return m_has_answer || (time_since_boot_ms() > m_timeout_timestamp);
}

}
