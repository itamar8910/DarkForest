#include "VgaDevice.h"
#include "IO.h"
#include "MM/MemoryManager.h"

#define MAX_RESOLUTION_WIDTH 4096
#define MAX_RESOLUTION_HEIGHT 2160

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA 0x01CF

#define VBE_DISPI_INDEX_ID 0x0
#define VBE_DISPI_INDEX_XRES 0x1
#define VBE_DISPI_INDEX_YRES 0x2
#define VBE_DISPI_INDEX_BPP 0x3
#define VBE_DISPI_INDEX_ENABLE 0x4
#define VBE_DISPI_INDEX_BANK 0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH 0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 0x7
#define VBE_DISPI_INDEX_X_OFFSET 0x8
#define VBE_DISPI_INDEX_Y_OFFSET 0x9
#define VBE_DISPI_DISABLED 0x00
#define VBE_DISPI_ENABLED 0x01
#define VBE_DISPI_LFB_ENABLED 0x40

constexpr u16 VGA_WIDTH = 1280;
constexpr u16 VGA_HEIGHT = 720;

VgaDevice::VgaDevice(const Path& path)
    : Device(path),
      m_width(VGA_WIDTH),
      m_height(VGA_HEIGHT),
      m_pitch(VGA_WIDTH * sizeof(u32))
{
    initialize();
}
static void set_register(u16 index, u16 data)
{
    IO::out16(VBE_DISPI_IOPORT_INDEX, index);
    IO::out16(VBE_DISPI_IOPORT_DATA, data);
}

void VgaDevice::initialize()
{
    set_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    set_register(VBE_DISPI_INDEX_XRES, m_width);
    set_register(VBE_DISPI_INDEX_YRES, m_height);
    set_register(VBE_DISPI_INDEX_VIRT_WIDTH, m_width);
    set_register(VBE_DISPI_INDEX_VIRT_HEIGHT, m_height * 2);
    set_register(VBE_DISPI_INDEX_BPP, 32);
    set_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
    set_register(VBE_DISPI_INDEX_BANK, 0);
}

int VgaDevice::mmap(void* virtual_addr, u32 size)
{
    const size_t fb_size = m_pitch * m_height * 2;
    if(size != fb_size)
    {
        return E_INVALID_SIZE;
    }

    // linear framebuffer
    void* lfb = (void*)0xf8000000;
    return MemoryManager::the().map((u32)virtual_addr, PhysicalAddress((u32)lfb), fb_size, PageWritable::YES, UserAllowed::YES);
}

int VgaDevice::ioctl(u32 request, void* buf)
{
    (void)request;
    (void)buf;
    // TODO: add get_dimensions code
    return E_NOT_SUPPORTED;
}