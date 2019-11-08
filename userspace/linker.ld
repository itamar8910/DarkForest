
/* userspace processes are loaded to 10MB */
base = 0xa00000;

ENTRY(_start);

SECTIONS
{

	. = base;
 
	.text ALIGN(4K) : AT (ADDR (.text) - base)
	{
		*(.text.prologue)
		*(.text)
	}
 
	/* Read-only data. */
	.rodata ALIGN(4K) : AT (ADDR (.rodata) - base)
	{
		*(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT (ADDR (.data) - base)
	{
		*(.data)
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) : AT (ADDR (.bss) - base)
	{
		*(COMMON)
		*(.bss)
	}
 
}
