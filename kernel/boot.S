; Declare constants for the multiboot header
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

KERNEL_BASE_PHYSICAL equ 0x100000
KERNEL_BASE_VIRTUAL equ 0x100000 ; TODO: when we move to higher half, set this to 3GB+1MB
 
; Declare a multiboot header that marks the program as a kernel
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
 
 ; initial stack
section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:

; paging tables - PD and first PT
; the addresses of page tables have to be 4KB aligned
section .bss
align 4096
page_directory:
resb 4096
PT_0:
resb 4096
 
section .text
global _start:function (_start.end - _start)
_start:
 
	; setup the stack
	mov esp, stack_top
 
	; setup paging
	; identity map the first 4MB
	; setup PT1 to point to 0...4MB
	mov edi, 0 ; current pysical address
	mov esi, PT_0 - KERNEL_BASE_VIRTUAL + KERNEL_BASE_PHYSICAL ; current PT1 entry, physical address
	.l1:
	mov edx, edi
	or edx, 0x3 ; set present, writable
	mov [esi], edx ; write current page table entry
	add esi, 4
	add edi, 4096
	cmp edi, 0x400000 ; while edi < 4MB
	jl _start.l1

    mov esi, PT_0 - KERNEL_BASE_VIRTUAL + KERNEL_BASE_PHYSICAL ; PT1, physical address
	mov edi, page_directory - KERNEL_BASE_VIRTUAL + KERNEL_BASE_PHYSICAL ; page directory physical address
	or esi, 0x03 ; set present, writable
	or [edi], esi

	; load addr of page directory to cr3
	mov esi, page_directory
	mov cr3, esi
	; enable paging
	mov esi, cr0
	or esi, 0x80000001
	mov cr0, esi

	; far jump TODO: (will only be needed in high half kernel)
	lea ecx, [_start.l2]
	jmp ecx
	.l2:

	; Call the global constructors
	push eax ; dummy value for alignment
	push eax ; dummt value for alignment
	push eax ; multiboot magic
	push ebx ; multiboot struct
	; the stack is now 16-byte aligned

	extern _init
	call _init

	extern kernel_main
	call kernel_main
 
	; If the system has nothing more to do, put the computer into an
	; infinite loop. To do that:
	; 1) Disable interrupts with cli (clear interrupt enable in eflags).
	;    They are already disabled by the bootloader, so this is not needed.
	;    Mind that you might later enable interrupts and return from
	;    kernel_main (which is sort of nonsensical to do).
	; 2) Wait for the next interrupt to arrive with hlt (halt instruction).
	;    Since they are disabled, this will lock up the computer.
	; 3) Jump to the hlt instruction if it ever wakes up due to a
	;    non-maskable interrupt occurring or due to system management mode.
	cli
.hang:	hlt
	jmp .hang
.end:
