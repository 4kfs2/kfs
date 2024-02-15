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

void mem_init()
{
	mmi.nframes = mmi.totsz / 0x1000;
	mmi.frames = var_partition(IDX_FRAME(mmi.nframes));
	memset(mmi.frames, 0, IDX_FRAME(mmi.nframes));
	for (uint32_t i = 0x0; i < (uint32_t)&_mapping_size; i += 0x1000U)
	{
		set_bit(i);
	}
	// test();
	kheap = init_heap(0x10000, 0xFFFFE000, 0, 1);
	init_vmpool(VMALLOC_START, VMALLOC_END);
}
