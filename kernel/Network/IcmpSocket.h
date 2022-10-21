#pragma once

#include "file.h"
#include "types/vector.h"
#include "Socket.h"

namespace Network
{

class IcmpSocket : public Socket
{
public:
    IcmpSocket() = default;
    ~IcmpSocket() override = default;

    int read(size_t count, void* buf) override;
    int write(char* data, size_t count) override;


    int sendto(SendToArgs& args) override;

private:
    Vector<Vector<u8>> m_pending_messages;
};

}