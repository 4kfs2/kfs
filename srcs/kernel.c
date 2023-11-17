#include "../includes/utils.h"
#include "../includes/terminal.h"
#include "../includes/elf.h"

void kernel_main(unsigned long addr)
{
	terminal_initialize();
	terminal_writestring("42\n");

	parse_elf(addr);
	keyboard();
}
