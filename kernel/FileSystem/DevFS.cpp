#include "DevFS.h"

#include "HAL/KeyboardDevice.h"
#include "HAL/VgaTextDevice.h"
#include "HAL/VgaTTY.h"
#include "HAL/AsciiKeyboardDevice.h"
#include "HAL/VgaDevice.h"
#include "HAL/MouseDevice.h"

static DevFS* s_the;

DevFS& DevFS::the() {
    if(!s_the){
        s_the = new DevFS();
    }
    return *s_the;
}

void DevFS::initiailize() {
    DevFS::the().add_device(new KeyboardDevice(Path("keyboard")));
    DevFS::the().add_device(new VgaTextDevice(Path("vgatext")));
    DevFS::the().add_device(new VgaTTYDevice(Path("vgatty")));
    DevFS::the().add_device(new AsciiKeyboardDevice(Path("asciiKeyboard")));
    DevFS::the().add_device(new VgaDevice(Path("vga")));
    DevFS::the().add_device(new MouseDevice(Path("mouse")));
}

void DevFS::add_device(Device* device) {
    devices.append(device);
}

File* DevFS::open(const Path& path) {
    ASSERT(path.num_parts() == 1);
    auto path_str = path.to_string();
    for(size_t i = 0; i < devices.size(); i++) {
        auto& dev = *devices[i];
        if(dev.path().to_string() == path_str) {
            return &dev;
        }
    }
    return nullptr;
}

bool DevFS::list_directory(const Path& path, Vector<DirectoryEntry>& res)
{
    for (auto dev : devices)
    {
        if (path.num_parts() == 0)
        {
            res.append(DirectoryEntry(dev->path(), DirectoryEntry::Type::File));
            continue;
        }
        if (path.to_string() == dev->path().to_string())
        {
            res.append(DirectoryEntry(dev->path(), DirectoryEntry::Type::File));
        }
    }
    return true;
}

bool DevFS::is_directory(const Path& path)
{
    (void)path;
    return false;
}

bool DevFS::is_file(const Path& path)
{
    (void)path;
    return false;
}

bool DevFS::create_entry(const Path& path, DirectoryEntry::Type type)
{
    (void)path;
    (void)type;
    NOT_IMPLEMENTED();
}

