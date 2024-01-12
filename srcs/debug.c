#include "../includes/debug.h"

struct symTabMeta symtab;

static uint64_t get_next_addr(uint32_t eip)
{
	uint32_t *ptr = (uint32_t*)(eip - 4);

	int offset = (int)(*ptr);
	return (offset + eip);
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

void backtrace()
{
	struct stackframe *stk;

	asm ("movl %%ebp,%0" : "=r" (stk) ::);
	for (int frame = 0; stk; ++frame)
	{
		uint32_t next_addr = get_next_addr(stk->eip);
		printf("[%d] %x(%x : %s)\n", frame, stk->eip, next_addr, find_symbol(next_addr));
		stk = stk->ebp;
	}
}
