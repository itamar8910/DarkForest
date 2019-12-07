#include "DevFS.h"

#include "HAL/KeyboardDevice.h"
#include "HAL/VgaTextDevice.h"
#include "HAL/VgaTTY.h"
#include "HAL/AsciiKeyboardDevice.h"

static DevFS* s_the;

DevFS& DevFS::the() {
    if(!s_the){
        s_the = new DevFS();
    }
    return *s_the;
}

void DevFS::initiailize() {
    DevFS::the().add_device(new KeyboardDevice("keyboard"));
    DevFS::the().add_device(new VgaTextDevice("vgatext"));
    DevFS::the().add_device(new VgaTTYDevice("vgatty"));
    DevFS::the().add_device(new AsciiKeyboardDevice("asciiKeyboard"));
}

void DevFS::add_device(Device* device) {
    devices.append(device);
}

File* DevFS::open(const String& path) {
    for(size_t i = 0; i < devices.size(); i++) {
        auto& dev = *devices[i];
        if(dev.path() == path) {
            return &dev;
        }
    }
    return nullptr;
}