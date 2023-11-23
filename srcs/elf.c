#include "../includes/elf.h"
#include "../includes/multiboot2.h"
#include "../includes/utils.h"

struct symTabMeta symtab;

void parse_elf(unsigned long addr)
{
    struct multiboot_tag *tag;

    for (tag = (struct multiboot_tag *)(addr + 8);
	     tag->type != MULTIBOOT_TAG_TYPE_END;
	     tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
    {
		//printf("Tag %d, Size %x\n", tag->type, tag->size);
		if (tag->type == MULTIBOOT_TAG_TYPE_ELF_SECTIONS)
		{
			struct multiboot_tag_elf_sections *elf_sections = tag;
			Elf32_Shdr *sections = (Elf32_Shdr *)elf_sections->sections;
			for (int i = 0; i < elf_sections->num; ++i)
			{
				if (sections[i].sh_type == SHT_SYMTAB || sections[i].sh_type == SHT_DYNAMIC)
				{
					symtab.index = i;
					symtab.sections = sections;
				}
			}
		}
    }
}

uint32_t find_symbol(uint32_t addr)
{
	int i = symtab.index;
	Elf32_Shdr *sections = symtab.sections;

	uint32_t num_symbols = sections[i].sh_size / sections[i].sh_entsize;
	Elf32_Sym *symbols = (Elf32_Sym *)(uintptr_t)(sections[i].sh_addr);

	for (uint32_t j = 0; j < num_symbols; ++j)
	{
		if (symbols[j].st_name != 0 && symbols[j].st_value == addr)
				return (uint32_t)(sections[sections[i].sh_link].sh_addr + symbols[j].st_name);
	}
	return NULL;
}
