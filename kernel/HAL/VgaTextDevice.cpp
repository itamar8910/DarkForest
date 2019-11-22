#include "VgaTextDevice.h"
#include "drivers/VgaText.h"
#include "asserts.h"

int VgaTextDevice::read(size_t count, void* buf) {
    (void)count;
    (void)buf;
    NOT_IMPLEMENTED("VgaTextDevice::read");
}
int VgaTextDevice::write(char* data, size_t count) {
    (void)data;
    (void)count;
    NOT_IMPLEMENTED("VgaTextDevice::write");
}
int VgaTextDevice::ioctl(u32 request, void* buf) {
    IOCTL_DATA* data = static_cast<IOCTL_DATA*>(buf);
    // bounds check
    if(data->row >= VgaText::VGA_HEIGHT
        || data->col >= VgaText::VGA_WIDTH) {
            return static_cast<int>(IOCTL_ERRS::E_OUT_OF_BOUNDS);
        }
    switch(static_cast<IOCTL_CODE>(request)) {
        case IOCTL_CODE::PUT_CHAR:
              unsigned char c;
              u8 color;
              VgaText::decompose_entry(data->value, c, color);
            VgaText::putchar(c, color, data->col, data->row);
            break;
        case IOCTL_CODE::MOVE_CURSOR:
            NOT_IMPLEMENTED("move cursor");
            break;
        case IOCTL_CODE::GET_CHAR:
            {
            u16 entry = VgaText::get_entry(data->col, data->row);
            unsigned char c;
            u8 color;
            VgaText::decompose_entry(data->value, c, color);
            data->value = static_cast<u16>(c);
            }
            break;
        case IOCTL_CODE::CLEAR:
            VgaText::clear(static_cast<u8>(data->value));
            break;
        default:
            ASSERT_NOT_REACHED("VgaTextDevice: invalid IOCTL code");
    }
    return 0;
}