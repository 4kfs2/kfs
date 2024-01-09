#include "../includes/terminal.h"
#include "../includes/multiboot2.h"
#include "../includes/elf.h"
#include "../includes/gdt.h"

void init();

void kernel_main(unsigned long addr)
{
	// init_gdt();
	terminal_initialize();
	//parse_elf(addr + 0x80000000);
	//parse_bootinfo(addr);
	init();
}
