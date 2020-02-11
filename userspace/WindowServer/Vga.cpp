#include "Vga.h"
#include "unistd.h"
#include "ioctl_common.h"
#include "asserts.h"
#include "kernel/errs.h"
#include "string.h"

constexpr u32 FRAME_BUFFER_MAP_ADDR = 0x60000000;
constexpr char VGA_PATH[] = "/dev/vga";

VGA::VGA()
{
    m_vga_fd = std::open(VGA_PATH);
    ASSERT(m_vga_fd >= 0);
    IOCTL::VGA::Data data = {};
    const int rc = std::ioctl(m_vga_fd, static_cast<u32>(IOCTL::VGA::Code::GET_DIMENSIONS), &data);
    ASSERT(rc == E_OK);

    m_width = data.width;
    m_height = data.height;
    m_pitch = data.pitch;

    const u32 size = m_pitch*m_height*2;
    void* const addr = (void*) FRAME_BUFFER_MAP_ADDR;
    const int map_rc  = std::map_device(m_vga_fd, addr, size);
    kprintf("done map\n");
    ASSERT(map_rc == E_OK);
    m_frame_buffer = (u32*) FRAME_BUFFER_MAP_ADDR;
}

VGA::~VGA()
{
    // TODO: imlement
    // unmap & close device
}

void VGA::clear()
{
    memset(m_frame_buffer, 0, height() * pitch()*2);
}

void VGA::draw(u32* window_buffer)
{
    memcpy(m_frame_buffer, window_buffer, height() * pitch());
}