#include "VgaTextCommon.h"
namespace VgaTextCommon {
    uint8_t compose_color(VgaColor fg, VgaColor bg) {
        return fg | bg << 4;
    }
    
    uint16_t compose_entry(unsigned char uc, uint8_t color) {
        return (uint16_t) uc | (uint16_t) color << 8;
    }

    void decompose_entry(uint16_t entry, unsigned char& uc, uint8_t& color) {
        uc = entry & 0xff;
        color = (entry>>8) & 0xffff;
    }
};