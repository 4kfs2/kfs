#include "../includes/utils.h"
#include "../includes/terminal.h"
#include "desc.h"

void kernel_main(void) 
{
	terminal_initialize();
	terminal_writestring("42\n");
	create_descriptor(0, 0, 0);
    create_descriptor(0, 0x000FFFFF, (GDT_CODE_PL0));
    create_descriptor(0, 0x000FFFFF, (GDT_DATA_PL0));
    create_descriptor(0, 0x000FFFFF, (GDT_CODE_PL3));
    create_descriptor(0, 0x000FFFFF, (GDT_DATA_PL3));
	// keyboard();
}
