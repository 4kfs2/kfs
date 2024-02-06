#include "../includes/mm.h"
#include "utils.h"
#include "panic.h"
#include "ordered_array.h"

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
		if (mmi.frames[idx] != 0xffffffff)
		{
			for (off = 0; off < 32; off++)
			{
				uint32_t tmp = 0x1 << off;
				if ((mmi.frames[idx] & tmp) == 0)
					return idx*32+off;
			}
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
		uint32_t tmp = var_partition(sizeof(page_tbl_t)); // TODO kmalloc
		pgdir->entries[dir_off] = V2P(tmp) | 0x3;
		memset(pgdir->entries[dir_off], 0, 0x1000U);
	}
	return _page(addr);
}

static int8_t header_lessthan(void *a, void *b)
{
	return ((header_t *)a)->size < ((header_t *)b)->size;
}

heap_t *init_heap(uint32_t size, uint32_t max_addr, uint8_t is_user, uint8_t is_write)
{
	heap_t *heap = (heap_t *)var_partition(sizeof(heap_t));
	uint32_t arr_addr = var_partition(sizeof(type_t) * ARRAY_IDX_SIZE);

	heap->arr = init_ordered_array((void *)arr_addr, ARRAY_IDX_SIZE, header_lessthan);
	// arr_addr 영역 이후 부터 메모리 할당이 가능한 지점
	arr_addr += (sizeof(type_t) * ARRAY_IDX_SIZE);
	 // 페이지 4K를 위한 주소값 지정
	ALIGN(arr_addr);
	// heap에 할당 가능한 공간의 정보 초기화
	heap->start_addr = arr_addr;
	heap->end_addr = arr_addr + size;
	heap->max_addr = max_addr;
	heap->is_user = is_user;
	heap->is_write = is_write;
	// ordered_array 에 저장할 첫 번째 hole의 header와 footer
	header_t *first_hole = (header_t *)heap->start_addr;
	first_hole->size = size;
	first_hole->magic = HEAP_MAGIC;
	first_hole->is_hole = 1;
	footer_t *footer = (footer_t *)(heap->end_addr - sizeof(footer_t));
	footer->header = first_hole;
	footer->magic = HEAP_MAGIC;
	insert_ordered_array((type_t)first_hole, &heap->arr);

	return heap;
}
