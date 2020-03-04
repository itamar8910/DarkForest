ENTRY(_start)
 
SECTIONS
{
	/* Kernel is loaded at 1MB physical address by GRUB*/
	/* Kernel lives in 3GB+1MB virtual address (higher half kernel) */
	. = 0xC0100000;
 
	.text ALIGN(4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.multiboot)
		*(.text)
	}
 
	.rodata ALIGN(4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}
 
	.data ALIGN(4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}
 
	.bss ALIGN(4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
	}
}
