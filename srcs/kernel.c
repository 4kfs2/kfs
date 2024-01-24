#include "../includes/utils.h"
#include "../includes/terminal.h"
#include "../includes/elf.h"
#include "mm.h"

# define CMD_SIZE 256

extern unsigned long *pg_dir;
extern unsigned long *pg_tbl;
extern unsigned long boot_page_directory;
extern unsigned long _end;

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
	// init_gdt();
	terminal_initialize();
	parse_elf(P2V(addr));
	// loadPageDirectory(pg_dir);
	// enablePaging();
	printf("%x %x\n", &boot_page_directory, &_end);
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
