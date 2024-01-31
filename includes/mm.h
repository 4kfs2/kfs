#ifndef _MM_H
# define _MM_H

#include <stdint.h>

# define KERNEL_BASE 0xC0000000
# define SHIFT 12
# define PAGE_SIZE 4096
# define PAGE_MASK (~(PAGE_SIZE - 1))
# define PAGING20(x) (uint32_t) (x & ~PAGE_MASK)
# define P2V(x) (((uint32_t) x) + KERNEL_BASE)
# define V2P(x) (((uint32_t) x) - KERNEL_BASE)
#define IDX_FRAME(a) (a/(4*8))
#define OFFSET_FRAME(a) (a%(4*8))

struct meminfo
{
	int			mem_lower;
	int			mem_upper;
	uint64_t	totsz;

	uint32_t	*frames;
	uint32_t	nframes;
};

typedef struct page_tbl
{
	uint32_t entries[1024];
} page_tbl_t;

typedef struct page_dir
{
	uint32_t entries[1024];
} page_dir_t;

extern struct meminfo mmi;
extern uint32_t _kernel_end;

void		mem_init();
void		set_bit(uint32_t addr);
void		unset_bit(uint32_t addr);
uint32_t	test_bit(uint32_t addr);
uint32_t	var_partion(uint32_t size);
uint32_t	get_frame();
void		alloc_frame(uint32_t *page);
uint32_t	*get_page(uint32_t addr);

#endif
