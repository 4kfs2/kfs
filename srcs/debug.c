#include "../includes/debug.h"
#include "../includes/utils.h"

void backtrace(uint32_t max_frame)
{
	struct stackframe *stk;

	asm ("movl %%ebp,%0" : "=r" (stk) ::);
	for (unsigned int frame = 0; stk && frame < max_frame; ++frame)
	{
		printf("%x\n", stk->eip);
		stk = stk->ebp;
	}
}
