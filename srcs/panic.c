#include "terminal.h"
#include "utils.h"

void panic(const char* msg, size_t size)
{
	terminal_write("PANIC: ", 7);
	terminal_write(msg, size);
	for (;;)
		;
}

void panic_1(const char* msg)
{
	panic(msg, strlen(msg));
}
