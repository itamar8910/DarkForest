#include "Ethernet.h"
#include "cstring.h"
#include "bits.h"

namespace Network
{

void Ethernet::EthernetHeader::flip_endianness()
{
    ethertype = (Ethernet::EtherType) to_flipped_endianness((uint16_t)ethertype);
}

shared_ptr<Vector<u8>> Ethernet::build(const MAC destination,
                                    const MAC source,
                                    EtherType ethertype,
                                    const uint8_t* payload,
                                    size_t payload_size)
{
    EthernetHeader header {.destination={},.source={}, .ethertype=ethertype};
    memcpy(header.destination.data, destination.data, MAC_SIZE);
    memcpy(header.source.data, source.data, MAC_SIZE);

    size_t frame_size = sizeof(EthernetHeader) + payload_size;
    size_t padding = 0;
    static constexpr size_t MINIMUM_FRAME_SIZE = 64;

    if (frame_size < MINIMUM_FRAME_SIZE)
    {
        padding = MINIMUM_FRAME_SIZE - frame_size;
        frame_size = MINIMUM_FRAME_SIZE;
    }

    header.flip_endianness();

    shared_ptr<Vector<u8>> data(new Vector<u8>(frame_size));
    data->set_size(frame_size);

    memcpy(data->data(), &header, sizeof(EthernetHeader));
    memcpy(data->data() + sizeof(EthernetHeader), payload, payload_size);
    memset(data->data() + sizeof(EthernetHeader) + payload_size, 0, padding);

    return data;
}

}
