#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdint.h>
#include <stddef.h>

#include "elf.h"

struct stackframe
{
	struct stackframe* ebp;
	uint32_t eip;
};

struct symTabMeta
{
	int index;
	Elf32_Shdr *sections;
};

void		backtrace();
uint32_t	find_symbol(uint32_t);

extern struct symTabMeta symtab;

#endif
