#include "../includes/debug.h"
#include "../includes/utils.h"

static uint64_t get_next_addr(uint32_t eip)
{
	unsigned char buffer[5];
    unsigned char *address = (unsigned char *)(eip - 5);

    asm volatile (
        "mov %[addr], %%esi\n"
        "mov $5, %%ecx\n"
        "rep movsb\n"
        :
        : [addr] "r" (address), "D" (buffer)
        : "esi", "ecx", "memory"
    );

	int32_t offset = *((int32_t*)(buffer + 1));
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
