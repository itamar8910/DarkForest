
#include "VgaText.h"
#include "VgaTextCommon.h"
#include "IO.h"


 
void VgaText::clear(uint8_t color) {
    uint16_t* terminal_buffer = VgaText::VGA_TEXT_BUFF;
	for (size_t y = 0; y < VgaText::VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VgaText::VGA_WIDTH; x++) {
			const size_t index = y * VgaText::VGA_WIDTH + x;
			terminal_buffer[index] = VgaTextCommon::compose_entry(' ', color);
		}
	}
}

void VgaText::update_cursor(int x, int y)
{
	uint16_t pos = y * VgaText::VGA_WIDTH + x;
 
	IO::outb(0x3D4, 0x0F);
	IO::outb(0x3D5, (uint8_t) (pos & 0xFF));
	IO::outb(0x3D4, 0x0E);
	IO::outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
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
