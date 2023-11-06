#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include "vga.h"
#include "keyboard.h"
#include <stddef.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

void terminal_initialize(void);
void terminal_setcolor(uint8_t);
void terminal_putentryat(unsigned char, uint8_t, size_t, size_t);
void terminal_putchar(char);
void terminal_write(const char*, size_t);
void terminal_writestring(const char*);

#endif
