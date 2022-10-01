#include "Arp.h"
#include "drivers/RTL8139NetworkCard.h"
#include "cstring.h"
#include "Ethernet.h"
#include "bits.h"

namespace Network
{

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
        Request = 0x1
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

void Arp::send_arp_request(const IPV4 target_ip, const IPV4 sender_ip)
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

    memcpy(arp_data.sender_mac.data, RTL8139NetworkCard::the().mac(), MAC_SIZE);
    memcpy(arp_data.sender_ip_address.data, sender_ip.data, IPV4_SIZE);
    memcpy(arp_data.target_mac.data, BROADCAST_MAC.data, MAC_SIZE);
    memcpy(arp_data.target_ip_address.data, target_ip.data, IPV4_SIZE);

    arp_data.flip_endianness();

    auto ether = Ethernet::build(arp_data.target_mac, arp_data.sender_mac, Ethernet::EtherType::ARP, (u8*)&arp_data, sizeof(arp_data));

    RTL8139NetworkCard::the().transmit(ether->data(), ether->size());

    // auto arp_data[28] = ...
    // auto ether = Ethernet::build(src, dst, Type::Arp, arp_data)
    // transmit(ether)
    // // The NetworkManager forwards the packets to the matching layer, e.g Arp / IP.
    // auto blocker = ArpPacketBlocker(..., timeout=5sec) // Unblocks when get a response to the ARP request
    // Scheduler::the().block_current(blocker);
    // if (blocker.timed_out()) ...
    // auto response = blocker.packet()
    // return response
}

}