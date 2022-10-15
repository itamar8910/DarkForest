#include "IcmpSocket.h"

namespace Network
{

IcmpSocket::IcmpSocket()
    : File(Path::empty())
{
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

}