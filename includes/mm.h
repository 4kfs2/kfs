#ifndef _MM_H
# define _MM_H

# define PAGE_SIZE (unsigned int)1 << 12

extern unsigned long *pg_dir;
extern unsigned long *pg_tbl;
extern void paging_init();


#endif