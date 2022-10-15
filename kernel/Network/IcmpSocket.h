#pragma once

#include "file.h"
#include "types/vector.h"

namespace Network
{

class IcmpSocket : public File
{
public:
    IcmpSocket();
    ~IcmpSocket() override = default;


    int read(size_t count, void* buf) override;
    int write(char* data, size_t count) override;

private:
    Vector<Vector<u8>> m_pending_messages;
};

}