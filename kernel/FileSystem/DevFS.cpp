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

File* DevFS::open(const Path& path) {
    ASSERT(path.num_parts() == 1);
    auto path_str = path.to_string();
    for(size_t i = 0; i < devices.size(); i++) {
        auto& dev = *devices[i];
        if(dev.path() == path_str) {
            return &dev;
        }
    }
    return nullptr;
}

bool DevFS::list_directory(const Path& path, Vector<DirectoryEntry> res)
{
    (void)path;
    (void)res;
    return false;
}