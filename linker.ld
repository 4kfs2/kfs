ENTRY(_start42)
 
SECTIONS
{
	. = 1M;
 
	.text :
	{
		*(.multiboot)
		*(.text)
	}
 
	.rodata :
	{
		*(.rodata)
	}
 
	.data :
	{
		*(.data)
	}
 
	.bss :
	{
		*(COMMON)
		*(.bss)
	}
	
}