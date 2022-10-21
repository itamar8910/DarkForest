#include "IcmpSocket.h"
#include "Network/Icmp.h"

namespace Network
{

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
    IPV4 dest;
    dest.data[0] = args.dest_addr->sin_addr.s_addr>>24;
    dest.data[1] = (args.dest_addr->sin_addr.s_addr>>16) & 0xFF;
    dest.data[2] = (args.dest_addr->sin_addr.s_addr>>8) & 0xFF;
    dest.data[3] = (args.dest_addr->sin_addr.s_addr) & 0xFF;
    Icmp::fix_checksum_and_send(dest, (const u8*)args.buf, args.len);
    return args.len;
}

}