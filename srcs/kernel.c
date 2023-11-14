#include "../includes/utils.h"
#include "../includes/terminal.h"
#include "gdt.h"

void kernel_main(void) 
{
	init_gdt();
	terminal_initialize();
	terminal_writestring("42\n");
	// keyboard();
}
