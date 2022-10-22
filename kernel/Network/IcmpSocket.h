#pragma once

#include "file.h"
#include "types/list.h"
#include "Socket.h"
#include "NetworkTypes.h"
#include "lock.h"

namespace Network
{

class IcmpSocket : public Socket
{
public:
    IcmpSocket();
    ~IcmpSocket() override;

    int read(size_t count, void* buf) override;
    int write(char* data, size_t count) override;

    int sendto(SendToArgs& args) override;
    int recvfrom(RecvFromArgs& args) override;

    struct PacketAndSource
    {
        Vector<u8> packet;
        IPV4 source {};
    };

    bool is_packet_relevant(const u8* packet, size_t size, IPV4 source);
    void on_received(PacketAndSource);

    bool has_pending_packets() const { return !m_pending_packets.empty(); }

private:
    List<PacketAndSource> m_pending_packets;
    IPV4 m_destination {};
    u8 m_id {0};
    Lock m_lock;
};

}