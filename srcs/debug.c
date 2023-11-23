#include "../includes/debug.h"
#include "../includes/utils.h"

static uint64_t get_next_addr(uint32_t eip)
{
	uint32_t *ptr = (uint32_t*)(eip - 4);

	int offset = (int)(*ptr);
	return (offset + eip);
}

void backtrace()
{
	struct stackframe *stk;

	asm ("movl %%ebp,%0" : "=r" (stk) ::);
	for (int frame = 0; stk; ++frame)
	{
		uint32_t next_addr = get_next_addr(stk->eip);
		printf("[%d] %x(%x : %s)\n", frame, stk->eip, next_addr, find_symbol(next_addr));
		stk = stk->ebp;
	}
}
