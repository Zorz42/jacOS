[bits 32]
[extern graphics_entry] ; Define calling point. Must have same name as kernel.c 'main' function
global _start;
_start:
push ebx
call graphics_entry ; Calls the C function. The linker will know where it is placed in memory
jmp $
