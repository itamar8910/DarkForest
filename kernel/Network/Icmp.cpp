#include "Icmp.h"
#include "Checksum.h"
#include "IpV4.h"
#include "bits.h"

namespace Network
{

struct __attribute__((__packed__)) IcmpEchoHeader
{
    Icmp::Type type;
    uint8_t code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence_number;

    void flip_endianness();
};

void IcmpEchoHeader::flip_endianness()
{
    identifier = to_flipped_endianness(identifier);
    sequence_number = to_flipped_endianness(sequence_number);
}

void Icmp::send_ping(IPV4 destination, uint16_t id, uint16_t sequence_number)
{
    static constexpr size_t payload_size = 32;
    u8 icmp_data[sizeof(IcmpEchoHeader) + payload_size] = {};

    IcmpEchoHeader icmp_header {
        .type = Type::Request,
        .code=0,
        .checksum=0,
        .identifier=id,
        .sequence_number = sequence_number
    };

    icmp_header.flip_endianness();

    memcpy(icmp_data, &icmp_header, sizeof(icmp_header));
    memset(icmp_data + sizeof(icmp_header), (int)'A', payload_size);

    reinterpret_cast<IcmpEchoHeader*>(icmp_data)->checksum = internet_checksum(icmp_data, sizeof(icmp_data));
    IpV4::send(destination, IpV4::Protocol::ICMP, icmp_data, sizeof(icmp_data));
}

}