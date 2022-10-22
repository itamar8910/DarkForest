#include "IcmpSocket.h"
#include "Network/Icmp.h"
#include "Icmp.h"
#include "packets.h"
#include "errs.h"
#include "TaskBlocker.h"
#include "Scheduler.h"
#include "sleep.h"

namespace Network
{

IcmpSocket::IcmpSocket()
    : m_lock("IcmpSocket")
{
    Icmp::the().register_socket(*this);
}

IcmpSocket::~IcmpSocket()
{
    Icmp::the().unregister_socket(*this);
}

int IcmpSocket::read(size_t count, void* buf)
{
    (void)count;
    (void)buf;
    return 0;
}

int IcmpSocket::write(char* data, size_t count)
{
    (void)count;
    (void)data;
    return 0;

}

int IcmpSocket::sendto(SendToArgs& args)
{
    IPV4 dest = IPV4::from_u32(args.dest_addr->sin_addr.s_addr);

    m_destination = dest;

    const IcmpEchoHeader* echo_header = reinterpret_cast<const IcmpEchoHeader*>(args.buf);
    if (args.len >= sizeof(IcmpEchoHeader) && echo_header->type == IcmpEchoHeader::Type::Request)
    {
        m_id = echo_header->identifier;
    }

    Icmp::the().fix_checksum_and_send(dest, (const u8*)args.buf, args.len);
    return args.len;
}

bool IcmpSocket::is_packet_relevant(const u8* packet, size_t size, IPV4 source)
{
    if (source != m_destination)
    {
        kprintf("source mismatch. source: %s, m_dest: %s\n", source.to_string().c_str(), m_destination.to_string().c_str());
        return false;
    }
    if (size < sizeof(IcmpEchoHeader))
    {
        kprintf("size mismatch\n");
        return false;
    }

    const IcmpEchoHeader* echo_header = reinterpret_cast<const IcmpEchoHeader*>(packet);
    if (echo_header->type != IcmpEchoHeader::Type::Reply)
    {
        kprintf("unknown ICMP type\n");
        return false;
    }

    return echo_header->identifier == m_id;
}

void IcmpSocket::on_received(PacketAndSource packet_and_source)
{
    LOCKER(m_lock);
    m_pending_packets.append(move(packet_and_source));
}


class PendingPacketsBlocker : public TaskBlocker {
public:
    PendingPacketsBlocker(IcmpSocket& socket);
    virtual bool can_unblock() override;
    ~PendingPacketsBlocker() override = default;
    bool timed_out() const { return m_timed_out; }

private:
    const IcmpSocket& m_socket;
    u32 m_timeout_ms {0};
    bool m_timed_out {false};
};

bool PendingPacketsBlocker::can_unblock()
{
    if (time_since_boot_ms() > m_timeout_ms)
    {
        m_timed_out = true;
        return true;
    }
    return m_socket.has_pending_packets();
}

int IcmpSocket::recvfrom(RecvFromArgs& args)
{
    while (m_pending_packets.empty())
    {
        kprintf("IcmpSocket:: blocking until there's a pending packet");
        PendingPacketsBlocker blocker(*this);
        Scheduler::the().block_current(&blocker);
        if (blocker.timed_out())
        {
            return -E_TIMED_OUT;
        }
    }

    LOCKER(m_lock);
    kprintf("Icmp: there is a pending packet");

    ASSERT(!m_pending_packets.empty()); // TODO: Fix race..

    auto packet = m_pending_packets.front();
    if (args.len < packet.packet.size() || *args.addrlen < sizeof(sockaddr_in))
    {
        return -E_TOO_BIG;
    }
    m_pending_packets.pop_front();

    memcpy(args.buf, packet.packet.data(), packet.packet.size());

    args.dest_addr->sin_addr.s_addr = packet.source.to_u32();
    *args.addrlen = sizeof(sockaddr_in);
    return packet.packet.size();
}

static constexpr u32 ICMP_TIMEOUT_MS = 3000;
PendingPacketsBlocker::PendingPacketsBlocker(IcmpSocket& socket)
    : m_socket(socket),
      m_timeout_ms(time_since_boot_ms() + ICMP_TIMEOUT_MS)
{
}

}