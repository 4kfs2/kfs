#ifndef __MM_H_
#define __MM_H_

#include <stdint.h>

#define PAGE_SIZE 0xffff;

struct meminfo
{
	int			mem_lower;
	int			mem_upper;
	uint64_t	totsz;

	uint32_t	*frames;
	uint32_t	nframes;
};

extern struct meminfo mmi;
extern char _kernel_end;

void mem_init();

#endif
