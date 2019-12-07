#pragma once

#include "types/String.h"

class VgaTTY
{
public:
    static VgaTTY& the();
    void write(const String&);
    void putchar(char);
private:
    VgaTTY();
    int m_vga_tty_fd {-1};
};