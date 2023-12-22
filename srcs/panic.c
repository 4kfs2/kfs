#include "terminal.h"

void panic(const char* msg, size_t size)
{
	terminal_write("PANIC: ", 7);
	terminal_write(msg, size);
	for (;;)
		;
}
