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

static uint32_t contract(uint32_t new_size)
{
	ALIGN(new_size);
	if (new_size < HEAP_MIN_SIZE)
		new_size = HEAP_MIN_SIZE;

	uint32_t size = kheap->end_addr - kheap->start_addr;
	uint32_t i = size - 0x1000;
	while (i > new_size)
	{
		free_frame(get_page(kheap->start_addr + i));
		i -= 0x1000;
	}
	kheap->end_addr = kheap->start_addr + new_size;
	return new_size;
}

static void *alloc(uint32_t size, uint8_t align)
{
	uint32_t new_size = size + sizeof(header_t) + sizeof(footer_t); 
	uint32_t iter = find_hole(new_size, align);
	if (iter == -1)
	{
		uint32_t length = kheap->end_addr - kheap->start_addr;
		uint32_t end_addr = kheap->end_addr;
		
		expand(length + new_size);
		uint32_t new_length = kheap->end_addr - kheap->start_addr;
		iter = 0;
		uint32_t idx = -1;
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
		if (idx == -1)
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
			if ((uint32_t)footer + sizeof(footer_t) > kheap->end_addr)
			{
				uint32_t length = kheap->end_addr - kheap->start_addr;
				uint32_t size = (uint32_t)footer - kheap->end_addr + sizeof(footer_t);
				expand(length+size);
			}			
			footer->header = header;
			footer->magic = HEAP_MAGIC;
		}
		return alloc(size, align);
	}
	// hole -> block
	header_t *target_hole = (header_t *)select_ordered_array(iter, &kheap->arr);
	uint32_t hole_loc = (uint32_t)target_hole;
	uint32_t hole_size = target_hole->size;
	
	// 요청된 크기만큼 나눠준 후, hole의 크기가 header와 footer의 크기 합 보다 작은 경우
	if (hole_size - new_size < sizeof(header_t) + sizeof(footer_t))
	{
		size += hole_size - new_size;
		new_size = hole_size;
	}
	if (align && (hole_loc & 0xFFFFF000))
	{
		uint32_t new_loc = hole_loc - (hole_loc & 0xFFF) + 0x1000 - sizeof(header_t);
		target_hole->size = 0x1000 - (hole_loc & 0xFFF) - sizeof(header_t);
		footer_t *footer = (footer_t *)(new_loc - sizeof(footer_t));
		footer->header = target_hole;
		footer->magic = HEAP_MAGIC;
		hole_loc = new_loc;
		hole_size -= target_hole->size;
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

	// hole의 크기가 block의 크기보다 작을 때, hole 만들기
	if (hole_size - block_header->size > 0)
	{
		header_t *header = (header_t *)(hole_loc + block_header->size);
		header->is_hole = 1;
		header->magic = HEAP_MAGIC;
		header->size = hole_size - block_header->size;
		footer_t *footer = (footer_t *)(hole_loc + header->size - sizeof(footer_t));
		if ((uint32_t)footer + sizeof(footer_t) > kheap->end_addr)
		{
			uint32_t length = kheap->end_addr - kheap->start_addr;
			uint32_t size = (uint32_t)footer - kheap->end_addr + sizeof(footer_t);
			expand(length+size);
		}			
		footer->header = header;
		footer->magic = HEAP_MAGIC;
		insert_ordered_array((type_t)header, &kheap->arr);
	}
	return (void *)((uint32_t)block_header + sizeof(header_t));
}

void kfree(void *addr)
{
	if (addr == 0)
		return ;

	header_t *header = (header_t *)((uint32_t)addr - sizeof(header_t));
	footer_t *footer = (footer_t *)((uint32_t)header + header->size - sizeof(footer_t));

	header->is_hole = 1;
	uint8_t do_add = 1;

	// 왼쪽 hole 합치기
	footer_t *left_hole = (footer_t *)((uint32_t)header - sizeof(footer_t));
	if ((uint32_t)left_hole >= kheap->start_addr &&
		left_hole->magic == HEAP_MAGIC &&
		left_hole->header->is_hole)
	{
		uint32_t size = header->size;
		header = left_hole->header;
		footer->header = header;
		header->size += size;
		do_add = 0;
	}

	// 오른쪽 hole 합치기
	header_t *right_hole = (header_t *)((uint32_t)footer + sizeof(footer_t));
	if ((uint32_t)right_hole < kheap->end_addr &&
		right_hole->magic == HEAP_MAGIC &&
		right_hole->is_hole)
	{
		header->size += right_hole->size;
		footer = (footer_t *)((uint32_t)right_hole + right_hole->size - sizeof(footer_t));

		uint32_t iter = 0;
		while ((iter < kheap->arr.size) &&
				(select_ordered_array(iter, &kheap->arr) != (type_t)right_hole))
			iter++;

		delete_ordered_array(iter, &kheap->arr);
	}

	if ((uint32_t)footer + sizeof(footer_t) == kheap->end_addr)
	{
		uint32_t old_length = kheap->end_addr - kheap->start_addr;
		uint32_t new_length = contract((uint32_t)header - kheap->start_addr);

		if (header->size - (old_length - new_length) > 0)
		{
			header->size -= old_length - new_length;
			footer = (footer_t *)((uint32_t)header + header->size - sizeof(footer_t));
			footer->magic = HEAP_MAGIC;
			footer->header = header;
		}
		else
		{
			uint32_t iter = 0;
			while ((iter < kheap->arr.size) &&
					(select_ordered_array(iter, &kheap->arr) != (type_t)right_hole))
				iter++;

			if (iter < kheap->arr.size)
				delete_ordered_array(iter, &kheap->arr);
		}
	}
	if (do_add == 1)
		insert_ordered_array((type_t)header, &kheap->arr);
}

static void *kmalloc_init(uint32_t size, uint8_t align, uint32_t *phys)
{
	void *addr = alloc(size, align);
	if (phys)
	{
		uint32_t *page = (uint32_t *)get_page((uint32_t)addr);
		*phys = (*page) & 0xFFFFF000;
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

