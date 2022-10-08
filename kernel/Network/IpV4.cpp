#include "IpV4.h"
#include "NetworkManager.h"
#include "bits.h"
#include "Ethernet.h"

namespace Network
{

struct __attribute__((__packed__)) IpV4Header
{
    // Swapped because of endianness
    uint8_t header_length_in_words :4;
    uint8_t version :4;

    uint8_t dscp_ecn;
    uint16_t total_length;
    uint16_t id;

    uint16_t flags_and_fragment_offset;

    uint8_t ttl;
    IpV4::Protocol protocol;
    uint16_t header_checksum;
    IPV4 source_address;
    IPV4 destination_address;

    void flip_endianness();
};

void IpV4Header::flip_endianness()
{
    total_length = to_flipped_endianness(total_length);
    flags_and_fragment_offset = to_flipped_endianness(flags_and_fragment_offset);
    id = to_flipped_endianness(id);
}

static constexpr uint8_t DONT_FRAGMENT = 1<<1;
static constexpr uint8_t INITIAL_TTL = 64;

void IpV4::send(IPV4 destination, Protocol protocol, const u8* payload, size_t payload_size)
{
    static uint16_t ipv4_id = 0;

    auto packet_size = sizeof(IpV4Header) + payload_size;
    Vector<u8> packet(packet_size);
    packet.set_size(packet_size);

    IpV4Header ip_header {
        .header_length_in_words = 5, // 20 bytes
        .version = 4,

        .dscp_ecn = 0,
        .total_length = static_cast<uint16_t>(sizeof(IpV4Header) + payload_size),
        .id = ipv4_id++,

        .flags_and_fragment_offset = DONT_FRAGMENT<<13,

        .ttl = INITIAL_TTL,
        .protocol = protocol,
        .header_checksum = 0,
        .source_address = Network::NetworkManager::the().our_ip(),
        .destination_address = destination
    };

    ip_header.flip_endianness();

    memcpy(packet.data(), &ip_header, sizeof(IpV4Header));
    memcpy(packet.data() + sizeof(IpV4Header), payload, payload_size);

    uint32_t sum = 0;

    for (size_t index = 0; index < packet_size; index += 2)
    {
        uint16_t current = 0;
        if (index < packet_size - 1)
        {
            current = *reinterpret_cast<uint16_t*>(packet.data() + index);
        } else {
            current = packet[index]<<8;
        }
        sum += current;
        if (sum > 0xFFFF)
        {
            sum -= 0XFFFF;
        }

    }
    uint16_t sum_16bit = (uint16_t) sum;
    uint16_t checksum = ~sum_16bit;
    reinterpret_cast<IpV4Header*>(packet.data())->header_checksum = checksum;

    MAC target_mac = {};
    if (!NetworkManager::the().resolve_arp(NetworkManager::the().gateway_ip(), target_mac))
    {
        kprintf("IpV4: Failed to resolve MAC of gateway\n");
        return;
    }
    Ethernet::send(target_mac, NetworkManager::the().our_mac(), Ethernet::EtherType::IPV4, packet.data(), packet.size());
}

}