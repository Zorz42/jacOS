[org 0x7c00]
MODE_INFO_OFFSET equ 0x7e00 ; where the vesa mode info will be saved
RAM_INFO_OFFSET equ 0x7f00 ; where the ram info will be saved
KERNEL_OFFSET equ 0x9000

call main
jmp $

%include "boot/kernel_size.asm"
%include "boot/print.asm"
%include "boot/switch_pm.asm"
%include "boot/load_kernel.asm"
%include "boot/get_ram_info.asm"

[bits 16]
main:
    mov bp, 0x9000
    mov sp, bp
    
    mov bx, LOADING_KERNEL
    call print
    call print_nl
    ; dont forget: dl has drive number so dont change it
    mov ax, KERNEL_OFFSET
    mov cx, KERNEL_SECTORS_SIZE
    call load_kernel ; read the kernel from disk

    mov bx, INITIALISING
    call print
    call print_nl
    mov di, 0x7f00 ; ram info destination
    call get_ram_info ; get ram size (most importantly)
    
    ; get video mode info
    mov ax, 4f01h
    mov cx, 118h
    mov di, MODE_INFO_OFFSET
    int 10h
    
    ; set video mode
    mov ax, 4f02h
    mov bx, 118h
    int 10h
    
    jmp switch_to_pm ; jump to 32 bit protected mode

    ret

[bits 32]
begin_pm:

    call KERNEL_OFFSET ; give control to the kernel

    jmp $

LOADING_KERNEL db "Loading kernel into memory", 0
INITIALISING db "Initialising", 0

; boot magic
times 510 - ($-$$) db 0
dw 0xaa55
