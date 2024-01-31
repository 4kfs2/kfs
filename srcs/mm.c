#include "../includes/mm.h"
#include "utils.h"

extern uint32_t _mapping_size;
struct meminfo mmi;

void mem_init()
{
	mmi.nframes = mmi.totsz / 0x1000;
	mmi.frames = bitmap_alloc(IDX_FRAME(mmi.nframes));
	memset(mmi.frames, 0, IDX_FRAME(mmi.nframes));
	for (uint32_t i = 0x0; i < (uint32_t)&_mapping_size; i += 0x1000U)
	{
		set_bit(i);
	}
}
