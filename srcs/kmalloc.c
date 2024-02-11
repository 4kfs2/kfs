#include "utils.h"
#include "mm.h"

extern heap_t *kheap;

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