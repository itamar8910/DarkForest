#pragma once

#include "FileSystem.h"
#include "device.h"
#include "types/vector.h"

class DevFS: public FileSystem {
public:
    static DevFS& the();
    static void initiailize();
    File* open(const String& path) override;

    void add_device(Device* device);

private:
    DevFS() : FileSystem("/dev") {}
    Vector<Device*> devices;

};