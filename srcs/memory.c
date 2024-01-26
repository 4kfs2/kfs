
unsigned long *pg_dir;
unsigned long *pg_tbl;

void paging_init()
{
	for(unsigned i = 0; i < 1024; i++)
	{
		pg_dir[i] = (unsigned long)(pg_tbl + 1024 * i) | 0x3;
	}
	for(unsigned i = 0; i < 1024; i++)
	{
		pg_tbl[i] = (i * 0x1000) | 0x1;
	}
}