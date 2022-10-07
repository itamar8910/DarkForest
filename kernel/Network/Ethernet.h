
#pragma once

#include "types.h"
#include "constants.h"
#include "shared_ptr.h"
#include "types/vector.h"
#include "NetworkTypes.h"

namespace Network
{

class Ethernet final
{
public:
    Ethernet() = delete;

    enum class EtherType : uint16_t
    {
        ARP = 0x0806,
    };

    static void send(const MAC destination,
                                        const MAC source,
                                        EtherType ethertype,
                                        const uint8_t* payload,
                                        size_t payload_size);



    struct __attribute__((__packed__)) EthernetHeader
    {
        MAC destination;
        MAC source;
        Ethernet::EtherType ethertype;
        void flip_endianness();
    };

private:
    static shared_ptr<Vector<u8>> build(const MAC destination,
                                        const MAC source,
                                        EtherType ethertype,
                                        const uint8_t* payload,
                                        size_t payload_size);

};

}
