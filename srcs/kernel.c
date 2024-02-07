#include "../includes/terminal.h"
#include "../includes/multiboot2.h"
#include "../includes/elf.h"
#include "../includes/utils.h"
#include "../includes/debug.h"
#include "../includes/gdt.h"
#include "../includes/mm.h"

void init();

void parse_bootinfo(unsigned long addr)
{
	struct multiboot_tag *tag;

    for (tag = (struct multiboot_tag *)(addr + 8);
	     tag->type != MULTIBOOT_TAG_TYPE_END;
	     tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
    {
		//printf("Tag %d, Size %x\n", tag->type, tag->size);
		switch (tag->type)
		{
			case MULTIBOOT_TAG_TYPE_MMAP:
			{
				multiboot_memory_map_t *mmap;
				for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
					 (multiboot_uint8_t *) mmap < (multiboot_uint8_t *) tag + tag->size;
					 mmap = (multiboot_memory_map_t *) ((unsigned long) mmap + ((struct multiboot_tag_mmap *) tag)->entry_size))
				{
					if (mmap->type == 1) // type value of 1 == available RAM
					{
						uint64_t endarea = mmap->addr + mmap->len;
						mmi.totsz = endarea <= 0xffffffff && mmi.totsz < endarea ? endarea : mmi.totsz;
						printf ("base_addr = %x %x," " length = %x %x\n", (unsigned) (mmap->addr >> 32), (unsigned) (mmap->addr & 0xffffffff), (unsigned) (mmap->len >> 32), (unsigned) (mmap->len & 0xffffffff), (unsigned) mmap->type);
					}
				}
				if (mmi.totsz > 0xffffffff) mmi.totsz = 0xffffffff;
				printf("total size of memory: %x %x\n", (unsigned)(mmi.totsz >> 32), (unsigned)(mmi.totsz & 0xffffffff));
			}
			break;
			case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
			{
				struct multiboot_tag_elf_sections *elf_sections = tag;
				Elf32_Shdr *sections = (Elf32_Shdr *)elf_sections->sections;
				for (int i = 0; i < elf_sections->num; ++i)
					if (sections[i].sh_type == SHT_SYMTAB || sections[i].sh_type == SHT_DYNAMIC)
					{
						symtab.index = i;
						symtab.sections = sections;
					}
			}
			break;
			case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
			{
				mmi.mem_lower =  ((struct multiboot_tag_basic_meminfo *)tag)->mem_lower;
				mmi.mem_upper =  ((struct multiboot_tag_basic_meminfo *)tag)->mem_upper;
			}
			break;
		}
    }
}

void kernel_main(unsigned long addr)
{
	// init_gdt();
	terminal_initialize();
	parse_bootinfo(P2V(addr));
	mem_init();
	init();
}
