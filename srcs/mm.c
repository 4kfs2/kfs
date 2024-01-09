#include "../includes/mm.h"

struct meminfo mmi;

void mem_init()
{
	mmi.frames = &_kernel_end;
	mmi.nframes = mmi.totsz >> 12;
}
