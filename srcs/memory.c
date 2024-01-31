#include "../includes/mm.h"
#include "utils.h"
#include "panic.h"

uint32_t placement = &_kernel_end;
struct meminfo mmi;

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

uint32_t bitmap_alloc(uint32_t size)
{
	uint32_t start = placement;
	placement += size;
	return start;
}
