#include "IpV4.h"
#include "NetworkManager.h"
#include "bits.h"
#include "Ethernet.h"
#include "Checksum.h"
#include "Icmp.h"

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

    reinterpret_cast<IpV4Header*>(packet.data())->header_checksum = internet_checksum(packet.data(), sizeof(IpV4Header));

    MAC target_mac = {};
    if (!NetworkManager::the().resolve_arp(NetworkManager::the().gateway_ip(), target_mac))
    {
        kprintf("IpV4: Failed to resolve MAC of gateway\n");
        return;
    }
    Ethernet::send(target_mac, NetworkManager::the().our_mac(), Ethernet::EtherType::IPV4, packet.data(), packet.size());
}

void IpV4::on_packet_received(u8* packet, size_t size)
{
    IpV4Header* header = reinterpret_cast<IpV4Header*>(packet);
    header->flip_endianness();

    if (header->destination_address != NetworkManager::the().our_ip())
    {
        kprintf("Received IP packet for unknown destination\n");
        return;
    }

    const auto header_size = header->header_length_in_words * sizeof(u32);
    u8* packet_after_ip_header = packet + header_size;
    size_t packet_size_after_ip_header = size - header_size;

    switch (header->protocol)
    {
        case IpV4::Protocol::ICMP:
            Icmp::the().on_packet_received(packet_after_ip_header, packet_size_after_ip_header, header->source_address);
            break;
        default:
            kprintf("Unknown IP protocol: %d\n", header->protocol);
            return;
    }
}

}