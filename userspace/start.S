global  _start
extern main

USER_STACK    equ  0xb0000000
PAGE_SIZE    equ  4096
ARGV_PTR    equ USER_STACK + PAGE_SIZE + 4
ARGC_PTR    equ USER_STACK + PAGE_SIZE + 8

section .text.prologue

_start:
; call main(argv, argc)
mov eax, [ARGC_PTR]
push eax
mov eax, [ARGV_PTR]
push eax
call main
mov ebx, eax ; return value
mov eax, 5 ; sycall exit
int 0x80

section .text