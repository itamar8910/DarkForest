#include "VgaTTY.h"
#include "df_unistd.h"
#include "asserts.h"

VgaTTY* s_the = nullptr;

VgaTTY& VgaTTY::the() {
    if(!s_the) {
        s_the = new VgaTTY();
    }
    return *s_the;
}

VgaTTY::VgaTTY() {
    m_vga_tty_fd = std::open("/dev/vgatty");
    ASSERT(m_vga_tty_fd>=0);
}

void VgaTTY::write(const String& str) {
    std::write(m_vga_tty_fd, str.c_str(), str.len());
}

void VgaTTY::putchar(char c) {
    std::write(m_vga_tty_fd, &c, 1);
}
