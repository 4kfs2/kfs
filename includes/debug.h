#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdint.h>

struct stackframe
{
	struct stackframe* ebp;
	uint32_t eip;
};

void backtrace();

#endif
