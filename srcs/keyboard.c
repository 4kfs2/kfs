#include "terminal.h"

#define KEYBOARD_COMMAND_PORT	0x64
#define KEYBOARD_DATA_PORT		0x60

static uint8_t inb(uint16_t port)
{
	uint8_t data;
	asm volatile ("inb %1, %0" : "=a" (data) : "dN" (port));
	return data;
}

static void outb(uint16_t port, uint8_t data)
{
	asm volatile ("outb %0, %1" :: "a" (data), "dN" (port));
}

void update_cursor(int x, int y)
{
	uint16_t pos = y * VGA_WIDTH + x;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void init_keyboard()
{
	outb(KEYBOARD_COMMAND_PORT, 0xAD);
	while (inb(KEYBOARD_COMMAND_PORT) & 0x01)
		inb(KEYBOARD_DATA_PORT);
	outb(KEYBOARD_COMMAND_PORT, 0xAE);
}

char map_scancode(uint8_t scancode)
{
	 static const char map[] = {
		0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
		'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
		'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
		'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
		0, ' ',
		0, 0, 0, 0, 0, 0, 0, 0,
		'7', '8', '9', '-',
		'4', '5', '6', '+',
		'1', '2', '3', '0', '.',
		0, 0, 0,
		0, 0, 0, 0, 0, 0, 0
	};

	if (scancode >= 0x80) {
		return 0;
	}
	return map[scancode];
}

int getchar()
{
	char scancode;

	while (1) {
		scancode = inb(0x60);
		if (scancode)
			break ;
	}
	return map_scancode(scancode);
}

void keyboard()
{
	int b = 0;
	while (1)
	{
		char c = getchar();
		if (c && b!=c) {
			b = c;
			terminal_putchar(c);
		}
	}
}
