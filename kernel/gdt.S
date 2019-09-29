
global gdt_flush:function (gdt_flush.end - gdt_flush)

gdt_flush:
   mov eax, [esp+4]  ; Get the pointer to the GDT, passed as a parameter.
   lgdt [eax]        ; Load the new GDT pointer
   ; the segment selector registers hold indices of the gdt.
   ; since each gdt entry has size of 8 bytes:
   ; 0x10 = 3rd entry = kernel data segment
   ; so we load idx of kernel data segment into all SS regs except the code SS reg.
   mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
   mov ds, ax        ; Load all data segment selectors
   mov es, ax
   mov fs, ax
   mov gs, ax
   mov ss, ax
   ; doing a "far jump" implicitly sets the cs reg
   jmp 0x08:.flush   ; 0x08 is the offset to our code segment: Far jump!
.flush:
   ret
.end: