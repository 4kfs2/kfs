#include "../includes/terminal.h"
#include "../includes/utils.h"

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0x80000000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll(int line) {
	size_t idx;
	uint16_t ch;

	for(int x = 0; x < VGA_WIDTH; x++) {
		idx = line * (VGA_WIDTH) + x; 
		ch = terminal_buffer[idx];
		terminal_buffer[idx - VGA_WIDTH] = ch;
	}
}

void terminal_delete_last_line() {
	int x;
	size_t idx;
 
	for(x = 0; x < VGA_WIDTH; x++) {
		idx = (VGA_WIDTH) * (VGA_HEIGHT - 1) + x;
		terminal_buffer[idx] = vga_entry(' ', terminal_color);
	}
}

void terminal_putchar(char c) 
{
	int line;
	unsigned char uc = c;

	if (c == '\n') {
		terminal_row++;
		terminal_column = 0;
		goto out;
	} else if (terminal_column < 6 && c == 8) {
		return ;
	} else if (c == 8) {
		terminal_column--;
		uc = ' ';
	}
	terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
	if (c != 8)
		terminal_column++;
	if (terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		terminal_row++;
	}
out:
	if (terminal_row == VGA_HEIGHT) {
		for(line = 1; line <= VGA_HEIGHT - 1; line++)
			terminal_scroll(line);
		terminal_delete_last_line();
		terminal_row = VGA_HEIGHT - 1;
	}
	update_cursor(terminal_column, terminal_row);
}
 
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}
