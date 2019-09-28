
#include "VgaTextDriver.h"


uint8_t VgaText::compose_color(VgaText::VgaColor fg, VgaText::VgaColor bg) {
	return fg | bg << 4;
}
 
uint16_t VgaText::compose_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

void VgaText::decompose_entry(uint16_t entry, unsigned char& uc, uint8_t& color) {
	uc = entry & 0xff;
	color = (entry>>8) & 0xffff;
}
 
void VgaText::clear(uint8_t color) {
    uint16_t* terminal_buffer = VgaText::VGA_TEXT_BUFF;
	for (size_t y = 0; y < VgaText::VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VgaText::VGA_WIDTH; x++) {
			const size_t index = y * VgaText::VGA_WIDTH + x;
			terminal_buffer[index] = compose_entry(' ', color);
		}
	}
}

void VgaText::putchar(char c, uint8_t color, size_t x, size_t y) 
{
    uint16_t* terminal_buffer = VgaText::VGA_TEXT_BUFF;
	const size_t index = y * VgaText::VGA_WIDTH + x;
	terminal_buffer[index] = VgaText::compose_entry((unsigned char) c, color);
}

uint16_t VgaText::get_entry(size_t x, size_t y) {
    uint16_t* terminal_buffer = VgaText::VGA_TEXT_BUFF;
	const size_t index = y * VgaText::VGA_WIDTH + x;
	return terminal_buffer[index];

}