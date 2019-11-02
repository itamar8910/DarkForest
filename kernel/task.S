

; based on: https://wiki.osdev.org/Brendan's_Multi-tasking_Tutorial

extern current_TCB
extern the_tss
global asm_switch_to_task

STRUC ThreadControlBlock
.id:  RESB (4)
.ESP:   RESB (4)
.ESP0:   RESB (4)
.CR3: RESB (4)
.meta_data: RESB(4)
ENDSTRUC

;C declaration:
;   void switch_to_task(ThreadControlBlock *next);
;
asm_switch_to_task:
    
    cli ;  disable interrupts
 
    ;Save previous task's state
 
    push ebx
    push esi
    push edi
    push ebp
    
    ; if current_TCB is nullptr (for example, current task is dead)
    ; then don't save esp for the current task state
    mov eax, 0
    mov ecx, [current_TCB]
    cmp ecx, eax
    je .no_save_esp
    mov edi,[current_TCB]    ;edi = address of the previous task's "thread control block"
    mov [edi+ThreadControlBlock.ESP],esp         ;Save ESP for previous task's kernel stack in the thread's TCB
.no_save_esp:
 
    ;Load next task's state
 
    mov esi,[esp+(4+1)*4]         ;esi = address of the next task's "thread control block" (parameter passed on stack)
    mov [current_TCB],esi    ;Current task's TCB is the next task TCB
 
    mov esp,[esi+ThreadControlBlock.ESP]         ;Load ESP for next task's kernel stack from the thread's ThreadControlBlock
    mov eax,[esi+ThreadControlBlock.CR3]         ;eax = address of page directory for next task
    mov ebx,[esi+ThreadControlBlock.ESP0]        ;NOTE: sould be ESP0? ebx = address for the top of the next task's kernel stack
    mov [the_tss + 4],ebx            ;Adjust the ESP0 field in the TSS (used by CPU for for CPL=3 -> CPL=0 privilege level changes)
    mov ecx,cr3                   ;ecx = previous task's virtual address space
 
    cmp eax,ecx                   ;Does the virtual address space need to being changed?
    je .doneVAS                   ; no, virtual address space is the same, so don't reload it and cause TLB flushes
    mov cr3,eax                   ; yes, load the next task's virtual address space
.doneVAS:
 
    pop ebp
    pop edi
    pop esi
    pop ebx
    
    sti ; enable interrupts

    ret                           ;Load next task's EIP from its kernel stack