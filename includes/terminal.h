#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include "vga.h"
#include "keyboard.h"
#include <stddef.h>

# define CURSOR_INDEX_PORT		0x3D4
# define CURSOR_DATA_PORT		0x3D5

# define UPPERCURSOR 0x0F
# define LOWERCURSOR 0x0E

void terminal_initialize(void);
void terminal_setcolor(uint8_t);
void terminal_putentryat(unsigned char, uint8_t, size_t, size_t);
void terminal_putchar(char);
void terminal_write(const char*, size_t);
void terminal_writestring(const char*);

#endif
