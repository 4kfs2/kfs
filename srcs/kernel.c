#include "utils.h"
#include "terminal.h"

void kernel_main(void) 
{
	terminal_initialize();
	terminal_writestring("42\n");
}
