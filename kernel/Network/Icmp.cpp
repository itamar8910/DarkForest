#include "Icmp.h"
#include "Checksum.h"
#include "IpV4.h"
#include "bits.h"
#include "packets.h"

namespace Network
{

void flip_endianness(IcmpEchoHeader& icmp)
{
    icmp.identifier = to_flipped_endianness(icmp.identifier);
    icmp.sequence_number = to_flipped_endianness(icmp.sequence_number);
}

void Icmp::send_ping(IPV4 destination, uint16_t id, uint16_t sequence_number)
{
    static constexpr size_t payload_size = 32;
    u8 icmp_data[sizeof(IcmpEchoHeader) + payload_size] = {};

    IcmpEchoHeader icmp_header {
        .type = IcmpEchoHeader::Type::Request,
        .code=0,
        .checksum=0,
        .identifier=id,
        .sequence_number = sequence_number
    };

    flip_endianness(icmp_header);

    memcpy(icmp_data, &icmp_header, sizeof(icmp_header));
    memset(icmp_data + sizeof(icmp_header), (int)'A', payload_size);

    reinterpret_cast<IcmpEchoHeader*>(icmp_data)->checksum = internet_checksum(icmp_data, sizeof(icmp_data));
    IpV4::send(destination, IpV4::Protocol::ICMP, icmp_data, sizeof(icmp_data));
}

void Icmp::fix_checksum_and_send(IPV4 destination, const u8* buffer, size_t size)
{
    Vector<u8> copy(buffer, size);
    IcmpEchoHeader* icmp_header = reinterpret_cast<IcmpEchoHeader*>(copy.data());
    flip_endianness(*icmp_header);
    icmp_header->checksum = internet_checksum(copy.data(), size);
    IpV4::send(destination, IpV4::Protocol::ICMP, copy.data(), size);
}

}