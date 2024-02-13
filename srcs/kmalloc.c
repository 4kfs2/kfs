#include "utils.h"
#include "mm.h"
#include "panic.h"

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

static void expand(uint32_t new_size)
{
	if (new_size <= kheap->end_addr - kheap->start_addr)
		panic_1("expand: newsize overflow\n");

	ALIGN(new_size);

	if (kheap->start_addr > kheap->start_addr + new_size)
		panic_1("expand: overflow\n");

	uint32_t i = kheap->end_addr - kheap->start_addr;
	while (i < new_size)
	{
		alloc_frame(get_page(kheap->start_addr + i));
		i += PAGE_SIZE;
	}
	kheap->end_addr = kheap->start_addr + new_size;
}

static void *alloc(uint32_t size, uint8_t align)
{
	uint32_t new_size = size + sizeof(header_t) + sizeof(footer_t); 
	uint32_t iter = find_hole(size, align);
	if (iter == -1)
	{
		uint32_t length = kheap->end_addr - kheap->start_addr;
		uint32_t end_addr = kheap->end_addr;
		
		expand(length + size);
		uint32_t new_length = kheap->end_addr - kheap->start_addr;
		iter = 0;
		uint32_t idx = ARRAY_IDX_SIZE + 1;
		uint32_t val = 0x0; // block
		while (iter < kheap->arr.size)
		{
			uint32_t tar = (uint32_t)select_ordered_array(iter, &kheap->arr);
			if (tar > val) // 메모리 상에서 주소값이 가장 큰 요소를 확장
			{
				val = tar;
				idx = iter;
			}
			iter++;
		}
		if (idx == ARRAY_IDX_SIZE + 1)
		{
			header_t *header = (header_t *)end_addr;
			header->is_hole = 1;
			header->magic = HEAP_MAGIC;
			header->size = new_length - length;
			footer_t *footer = (footer_t *)((uint32_t)header + header->size - sizeof(footer_t));
			footer->header = header;
			footer->magic = HEAP_MAGIC;
			insert_ordered_array((type_t)header, &kheap->arr);
		}
		else
		{
			header_t *header = (header_t *)select_ordered_array(idx, &kheap->arr);
			header->size += new_length - length;
			footer_t *footer = (footer_t *)((uint32_t)header + header->size - sizeof(footer_t));
			footer->header = header;
			footer->magic = HEAP_MAGIC;
		}
		return alloc(size, align);
	}
	// hole -> block
	header_t *target_hole = (header_t *)select_ordered_array(iter, &kheap->arr);
	uint32_t hole_loc = (uint32_t)target_hole;
	uint32_t hole_size = target_hole->size;
	
	// 요청된 크기만큼 나눠준 후에 hole의 크기가 header와 footer의 크기 합 보다 작은 경우 
	if (hole_size - new_size < sizeof(header_t) + sizeof(footer_t))
	{
		size += hole_size - new_size;
		new_size = hole_size;
	}
	if (align && (hole_loc & 0xFFFFF000))
	{
		uint32_t new_loc = hole_loc - (hole_loc & 0xFFF) + 0x1000 - sizeof(header_t);
		header_t *header = (header_t *)hole_loc;
		header->size = 0x1000 - (hole_loc & 0xFFF) - sizeof(header_t);
		header->magic = HEAP_MAGIC;
		header->is_hole = 1;
		footer_t *footer = (footer_t *)(new_loc - sizeof(footer_t));
		footer->header = header;
		footer->magic = HEAP_MAGIC;
		hole_loc = new_loc;
		hole_size -= header->size;
	}
	else
	{
		delete_ordered_array(iter, &kheap->arr);
	}
	header_t *block_header = (header_t *)hole_loc;
	block_header->is_hole = 1;
	block_header->size = new_size;
	block_header->magic = HEAP_MAGIC;
	footer_t *block_footer = (footer_t *)(hole_loc + new_size - sizeof(footer_t));
	block_footer->header = block_header;
	block_footer->magic = HEAP_MAGIC;

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