#ifndef _MM_H
# define _MM_H

# define KERNEL_BASE 0xC0000000
# define SHIFT 12
# define PAGE_SIZE 4096
# define PAGE_MASK (~(PAGE_SIZE - 1))
# define PAGING20(x) (uint32_t) (x & ~PAGE_MASK)
# define P2V(x) (uint32_t) (x + KERNEL_BASE)
# define V2P(x) (uint32_t) (x - KERNEL_BASE)

extern void paging_init();

#endif