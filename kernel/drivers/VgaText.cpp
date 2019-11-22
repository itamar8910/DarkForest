
#include "VgaText.h"
#include "VgaTextCommon.h"


 
void VgaText::clear(uint8_t color) {
    uint16_t* terminal_buffer = VgaText::VGA_TEXT_BUFF;
	for (size_t y = 0; y < VgaText::VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VgaText::VGA_WIDTH; x++) {
			const size_t index = y * VgaText::VGA_WIDTH + x;
			terminal_buffer[index] = VgaTextCommon::compose_entry(' ', color);
		}
	}
}

void VgaText::putchar(char c, uint8_t color, size_t x, size_t y) 
{
    uint16_t* terminal_buffer = VgaText::VGA_TEXT_BUFF;
	const size_t index = y * VgaText::VGA_WIDTH + x;
	terminal_buffer[index] = VgaTextCommon::compose_entry((unsigned char) c, color);
}

uint16_t VgaText::get_entry(size_t x, size_t y) {
    uint16_t* terminal_buffer = VgaText::VGA_TEXT_BUFF;
	const size_t index = y * VgaText::VGA_WIDTH + x;
	return terminal_buffer[index];

}
