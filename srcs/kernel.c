#include "../includes/utils.h"
#include "../includes/terminal.h"
#include "../includes/elf.h"
#include "multiboot2.h"
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

void mmap(unsigned addr)
{
	struct multiboot_tag *tag;
	unsigned size;

	size = *(unsigned *) addr;
  	printf ("Announced mbi size 0x%x\n", size);
	for (tag = (struct multiboot_tag *) (addr + 8);
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
                                       + ((tag->size + 7) & ~7)))
    {
		switch (tag->type)
		{
		case MULTIBOOT_TAG_TYPE_MMAP:
			{
			multiboot_memory_map_t *mmap;

			printf ("mmap\n");
		
			for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
				 (multiboot_uint8_t *) mmap 
					 < (multiboot_uint8_t *) tag + tag->size;
				 mmap = (multiboot_memory_map_t *) 
					 ((unsigned long) mmap + ((struct multiboot_tag_mmap *) tag)->entry_size))
				printf (" base_addr = 0x%x%x,"
						" length = 0x%x%x, type = 0x%x (%s) \n", 
						(unsigned) (mmap->addr >> 32),
						(unsigned) (mmap->addr & 0xffffffff),
						(unsigned) (mmap->len >> 32),
						(unsigned) (mmap->len & 0xffffffff),
						(unsigned) mmap->type,
						mmap->type == 2 ? "reserved" : "available");
			}
			break;
		}
	}
  tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
                                  + ((tag->size + 7) & ~7));

  printf ("Total mbi size 0x%x\n", (unsigned) tag - addr);
}

void kernel_main(unsigned long addr)
{
	// init_gdt();
	terminal_initialize();
	printf("%x\n", addr);
	mmap(addr + 0x80000000);
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
