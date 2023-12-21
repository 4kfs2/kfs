#include "../includes/utils.h"
#include "../includes/terminal.h"
#include "../includes/elf.h"
#include "debug.h"

# define CMD_SIZE 256

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

void func2()
{
	backtrace();
}

void func()
{
	func2();
}

void kernel_main(unsigned long addr)
{
	// init_gdt();
	terminal_initialize();
	// printf("%x\n", addr);
	parse_elf(addr + 0x80000000);
	func();
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
