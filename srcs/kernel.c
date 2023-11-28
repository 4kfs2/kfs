#include "../includes/utils.h"
#include "../includes/terminal.h"
#include "../includes/elf.h"
#include "debug.h"

# define CMD_SIZE 256

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));
uint32_t second_page_table[1024] __attribute__((aligned(4096)));
uint32_t third_page_table[1024] __attribute__((aligned(4096)));

static void runcmd(int index, char *cmd)
{
	if (index)
	{
		if (strncmp(cmd, "reboot", index) == 0)
			reboot();
		else
			terminal_writestring("command not found :)\n");
	}
}

void kernel_main(unsigned long addr)
{
	init_gdt();
	terminal_initialize();

	parse_elf(addr);
	unsigned int i;
 
	for(i = 0; i < 1024; i++)
	{
		page_directory[i] = 0x00000002;
	}
	for(i = 0; i < 512; i++)
	{
		first_page_table[i] = (i * 0x1000) | 0x1;
	}
	for(i = 512; i < 1024; i++)
	{
		first_page_table[i] = (i * 0x1000) | 0x3;
	}
	for(i = 0; i < 256; i++)
	{
		second_page_table[i] = (i * 0x1000) | 0x1;
	}
	for(i = 256; i < 512; i++)
	{
		second_page_table[i] = (i * 0x1000) | 0x7;
	}
	for(i = 512; i < 768; i++)
	{
		second_page_table[i] = (i * 0x1000) | 0;
	}
	for(i = 768; i < 1024; i++)
	{
		second_page_table[i] = (i * 0x1000) | 0x3;
	}
	for(i = 0; i < 256; i++)
	{
		third_page_table[i] = (i * 0x1000) | 0x1;
	}
	for(i = 256; i < 512; i++)
	{
		third_page_table[i] = (i * 0x1000) | 0x3;
	}
	for(i = 512; i < 1024; i++)
	{
		third_page_table[i] = (i * 0x1000) | 0x7;
	}
	page_directory[0] = ((unsigned int)first_page_table) | 0x3;
	page_directory[1] = ((unsigned int)second_page_table) | 0x5;
	page_directory[2] = ((unsigned int)third_page_table) | 0x7;
	loadPageDirectory(page_directory);
	asm("	mov %cr0, %eax; \
       		or $0x80000000, %eax; \
			mov %eax, %cr0");
	test();
	while (1)
	{
		int index = 0;
		char cmd[CMD_SIZE] = {0};
		terminal_writestring("kfs$ ");
		while (1)
		{
			char c = getchar();
			if (c)
			{
				terminal_putchar(c);
				if (c == '\n')
				{
					runcmd(index, cmd);
					break;
				}
				cmd[index++] = c;
			}
		}
	}
}
