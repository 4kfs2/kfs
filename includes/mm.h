#ifndef _MM_H
# define _MM_H

#include <stdint.h>
#include "ordered_array.h"

# define KERNEL_BASE 0xC0000000
# define SHIFT 12
# define PAGE_SIZE 4096
# define PAGE_MASK (~(PAGE_SIZE - 1))
# define PAGING20(x) (uint32_t) (x & ~PAGE_MASK)
# define P2V(x) (((uint32_t) x) + KERNEL_BASE)
# define V2P(x) (((uint32_t) x) - KERNEL_BASE)
#define IDX_FRAME(a) (a/(4*8))
#define OFFSET_FRAME(a) (a%(4*8))
#define ALIGN(a) if (a & 0x00000FFF) { a &= 0xFFFFF000; a += 0x1000; }
#define ARRAY_IDX_SIZE 0x20000
#define HEAP_MAGIC 0xBABA1EFF

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
	uint32_t entries[1024]; // physical_addr
	page_tbl_t *tables[1024]; // virtual_addr
} page_dir_t;

typedef struct
{
	ordered_array_t	arr;
	uint32_t		start_addr;
	uint32_t		end_addr;
	uint32_t		max_addr;
	uint8_t			is_user; // 1 ? user: kernel;
	uint8_t			is_write; // 1 ? rw : r;
} heap_t;

typedef struct
{
	uint8_t is_hole;
	uint32_t magic;
	uint32_t size;
} header_t;

typedef struct
{
	uint32_t header;
	uint32_t magic;
} footer_t;

extern struct meminfo mmi;
extern uint32_t _kernel_end;

void		mem_init();
void		set_bit(uint32_t addr);
void		unset_bit(uint32_t addr);
uint32_t	test_bit(uint32_t addr);
uint32_t	var_partition(uint32_t size);
uint32_t	get_frame();
void		alloc_frame(uint32_t *page);
uint32_t	*get_page(uint32_t addr);
heap_t		*init_heap(uint32_t size, uint32_t max_addr, uint8_t is_user, uint8_t is_write);

#endif
