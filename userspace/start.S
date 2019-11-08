global  _start
extern main

section .text.prologue

_start:
call main
mov ebx, eax ; return value
mov eax, 5 ; sycall exit
int 0x80

section .text