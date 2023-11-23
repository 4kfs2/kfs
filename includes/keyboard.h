#ifndef __KEYBOARD_H__
# define __KEYBOARD_H__

# include <stdint.h>

# define KEYBOARD_COMMAND_PORT	0x64
# define KEYBOARD_DATA_PORT		0x60

# define KBRD_RESET 0xFE

void	update_cursor(int, int);

int		getchar();

void	outb(uint16_t, uint8_t);
uint8_t	inb(uint16_t);

#endif
