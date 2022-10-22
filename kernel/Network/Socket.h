#pragma once

#include "file.h"
#include "sys/socket.h"

namespace Network
{

class Socket : public File
{
protected:
    Socket() 
    : File(Path::empty()) {}


public:
    virtual ~Socket() override = default;

    int read(size_t count, void* buf) override = 0;
    int write(char* data, size_t count) override = 0;

    virtual int sendto(SendToArgs&) = 0;
    virtual int recvfrom(RecvFromArgs&) = 0;
};

}