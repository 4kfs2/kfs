ENTRY(_start42)
 
SECTIONS
{
	. = 1M;
 
	.text :
	{
		*(.text)
	}
}