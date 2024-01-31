#include "../includes/mm.h"
#include "utils.h"
#include "panic.h"

extern page_dir_t *pgdir;
uint32_t placement = &_kernel_end;

void set_bit(uint32_t addr)
{
	uint32_t frame = addr / 0x1000;
	uint32_t idx = IDX_FRAME(frame);
	uint32_t off = OFFSET_FRAME(frame);
	mmi.frames[idx] |= (0x1 << off);
}

void unset_bit(uint32_t addr)
{
	uint32_t frame = addr / 0x1000;
	uint32_t idx = IDX_FRAME(frame);
	uint32_t off = OFFSET_FRAME(frame);
	mmi.frames[idx] &= ~(0x1 << off);
}

uint32_t test_bit(uint32_t addr)
{
	uint32_t frame = addr / 0x1000;
	uint32_t idx = IDX_FRAME(frame);
	uint32_t off = OFFSET_FRAME(frame);
	return (mmi.frames[idx] & (0x1 << off));
}

uint32_t var_partition(uint32_t size)
{
	uint32_t start = placement;
	placement += size;
	return start;
}

uint32_t get_frame() // find frame
{
	uint32_t idx, off;
	for (idx = 0; idx < IDX_FRAME(mmi.nframes); idx++)
	{
		for (off = 0; off < 32; off++)
		{
			uint32_t tmp = 0x1 << off;
			if ((mmi.frames[idx] & tmp) == 0)
				return idx*32+off;
		}
	}
	return (uint32_t)-1;
}

void alloc_frame(uint32_t *page) // mapping page to frame
{
	if (*page == 0) // page에 아무런 내용이 없으면
	{
		uint32_t frame = get_frame();
		if (frame == (uint32_t)-1)
		{
			panic("no frame\n", strlen("no frame\n"));
		}
		set_bit(frame * 0x1000U);
		*page = (frame * 0x1000U) | 0x3;
	}
}

static uint32_t *_page(uint32_t addr)
{
	uint32_t addr20 = addr >> SHIFT;
	uint32_t dir_off = addr20 >> 10;
	uint32_t tbl_off = addr20 & 0x3ff;
	page_tbl_t *table = P2V(pgdir->entries[dir_off] & (~0xfff)); // 엔트리에 물리 주소가 저장되어 있으므로 
	return &table->entries[tbl_off];
}

uint32_t *get_page(uint32_t addr)
{
	uint32_t dir_off = addr >> (SHIFT + 10);
	if (!pgdir->entries[dir_off]) // 테이블이 없는 경우
	{
		uint32_t tmp = var_partion(sizeof(page_tbl_t)); // TODO kmalloc
		pgdir->entries[dir_off] = V2P(tmp) | 0x3;
		memset(pgdir->entries[dir_off], 0, 0x1000U);
	}
	return _page(addr);
}
