#include "../includes/mm.h"
#include "utils.h"

extern uint32_t _mapping_size;
extern uint32_t boot_page_directory;
struct meminfo mmi;
page_dir_t *pgdir = &boot_page_directory;
heap_t *kheap = 0;

void test()
{
	printf("%x\n", pgdir);
	uint32_t i;
	for (i = 0; i < 1024; i++) // 디렉토리에서 테이블 할당 여부 찾기
	{ 
		if (pgdir->entries[i]) 
		{
			printf("%x : %x\n", i, pgdir->entries[i]);
			break ;
		}
	}
	page_tbl_t *tmp = pgdir->tables[0x300];
	for (uint32_t i = 0; i < 10; i++) // 테이블에서 프레임 할당 여부 찾기
	{ 
			printf("%x %x\n",i, tmp->entries[i]);
	}
	alloc_frame(get_page(0xC0210000));
	alloc_frame(get_page(0xC0201000));
}

uint32_t find_hole(uint32_t size, uint8_t align)
{
	uint32_t iter = 0;
	while (iter < kheap->arr.size)
	{
		header_t *header = select_ordered_array(iter, &kheap->arr);
		if (align)
		{
			uint32_t pos = (uint32_t)header;
			int32_t offset = 0;
			if ((pos + sizeof(header_t)) & 0x00000FFF)
				offset = 0x1000 - ((pos + sizeof(header_t)) % 0x1000);
			// 정렬된 위치에 할당되어야함. 이용가능한 크기 - 정렬하느라 건너 뛴 크기
			int32_t hole_size = (int32_t)header->size - offset; 
			if ((int32_t)size <= hole_size)
				break;
		}
		else if (size <= header->size)
			break ;
		iter++;
	}
	if (iter == kheap->arr.size)
		return -1;
	return iter;
}

static void *alloc(uint32_t size, uint8_t align)
{
	uint32_t new_size = size + sizeof(header_t) + sizeof(footer_t); 
	uint32_t iter = find_hole(size, align);
	if (iter == -1)
	{
		expand();
		return alloc(size, align);
	}
}

static void *kmalloc_init(uint32_t size, uint8_t align, uint32_t *phys)
{
	void *addr = alloc(size, align);
	if (phys)
	{
		uint32_t page = (uint32_t)get_page((uint32_t)addr);
		*phys = page & 0xFFFFF000;
	}
	return addr;
}

void  *kmalloc(uint32_t size)
{
	return kmalloc_init(size, 0, 0);
}

void *kmalloc_ap(uint32_t size, uint32_t *phys)
{
	return kmalloc_init(size, 1, phys);
}

void mem_init()
{
	mmi.nframes = mmi.totsz / 0x1000;
	mmi.frames = var_partition(IDX_FRAME(mmi.nframes));
	memset(mmi.frames, 0, IDX_FRAME(mmi.nframes));
	for (uint32_t i = 0x0; i < (uint32_t)&_mapping_size; i += 0x1000U)
	{
		set_bit(i * 0x1000U);
	}
	// test();
	kheap = init_heap(0x10000, 0xFFFFE000, 0, 1);
}
