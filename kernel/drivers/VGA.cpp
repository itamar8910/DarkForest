#include "VGA.h"
#include "types.h"
#include "IO.h"
#include "MM/MemoryManager.h"
#include "string.h"

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



static void set_register(u16 index, u16 data)
{
    IO::out16(VBE_DISPI_IOPORT_INDEX, index);
    IO::out16(VBE_DISPI_IOPORT_DATA, data);
}

static void play()
{
    size_t width = 1280;
    size_t height = 720;
    u32 pitch = width * sizeof(u32);
    size_t fb_size = pitch * height * 2;

    void* lfb = (void*)0xf8000000;
    void* virt = (void*)0xe0000000;
    MemoryManager::the().map((u32)virt, PhysicalAddress((u32)lfb), fb_size, PageWritable::YES, UserAllowed::NO);
    u32* buff = (u32*)virt;
    for(size_t row = 0; row < height; ++row)
    {
        for(size_t col = 0; col < width; ++col)
        {
            u32* pixel = buff + (row*width + col);
            // kprintf("0x%x,", pixel);
            *pixel = (col%10 < 5) ? 0xFF0000FF : 0xFF00FF00;
        }
    }

    // memset(buff, 0x22220055, fb_size/sizeof(u32));
    // memset(buff, 0xFFFF00FF, fb_size);
   
}

void VGA::init()
{
    size_t width = 1280;
    size_t height = 720;

    set_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    set_register(VBE_DISPI_INDEX_XRES, (u16)width);
    set_register(VBE_DISPI_INDEX_YRES, (u16)height);
    set_register(VBE_DISPI_INDEX_VIRT_WIDTH, (u16)width);
    set_register(VBE_DISPI_INDEX_VIRT_HEIGHT, (u16)height * 2);
    set_register(VBE_DISPI_INDEX_BPP, 32);
    set_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
    set_register(VBE_DISPI_INDEX_BANK, 0);

    play();

    // dbg() << "BXVGADevice resolution set to " << m_framebuffer_width << "x" << m_framebuffer_height << " (pitch=" << m_framebuffer_pitch << ")";
}