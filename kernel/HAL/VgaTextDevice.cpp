#include "VgaTextDevice.h"
#include "drivers/VgaText.h"
#include "asserts.h"
#include "ioctl_common.h"
#include "VgaTextCommon.h"

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
    IOCTL::VgaText::Data* data = static_cast<IOCTL::VgaText::Data*>(buf);
    // bounds check
    if(data->row >= VgaText::VGA_HEIGHT
        || data->col >= VgaText::VGA_WIDTH) {
            return static_cast<int>(IOCTL::VgaText::Errs::E_OUT_OF_BOUNDS);
        }
    switch(static_cast<IOCTL::VgaText::Code>(request)) {
        case IOCTL::VgaText::Code::PUT_CHAR:
              unsigned char c;
              u8 color;
              VgaTextCommon::decompose_entry(data->value, c, color);
            VgaText::putchar(c, color, data->col, data->row);
            break;
        case IOCTL::VgaText::Code::MOVE_CURSOR:
            NOT_IMPLEMENTED("move cursor");
            break;
        case IOCTL::VgaText::Code::GET_CHAR:
            {
            u16 entry = VgaText::get_entry(data->col, data->row);
            unsigned char c;
            u8 color;
            VgaTextCommon::decompose_entry(entry, c, color);
            data->value = static_cast<u16>(c);
            }
            break;
        case IOCTL::VgaText::Code::GET_ENTRY:
            {
            u16 entry = VgaText::get_entry(data->col, data->row);
            data->value = static_cast<u16>(entry);
            }
            break;
        case IOCTL::VgaText::Code::UPDATE_CURSOR:
            {
                VgaText::update_cursor(data->col, data->row);
            }
            break;
        case IOCTL::VgaText::Code::CLEAR:
            VgaText::clear(static_cast<u8>(data->value));
            break;
        default:
            ASSERT_NOT_REACHED("VgaTextDevice: invalid IOCTL code");
    }
    return 0;
}