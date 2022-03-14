[org 0x7c00]
MODE_INFO_OFFSET equ 0x7e00 ; Where the vesa mode info will be saved
RAM_INFO_OFFSET equ 0x7f00 ; Where the ram info will be saved
KERNEL_OFFSET equ 0x9000

%include "boot/kernel_size.asm"

call main
jmp $

BOOT_DRIVE db 0

%include "boot/switch_pm.asm"
%include "boot/load_kernel.asm"
%include "boot/get_ram_info.asm"

[bits 16]
main:
    ; Store number of boot drive
    mov [BOOT_DRIVE], dl
    mov bp, 0x9000
    mov sp, bp

    ; Set video mode
    mov ax, 4f02h
    mov bx, 118h
    int 10h

    ; Get video mode info
    mov ax, 4f01h
    mov cx, 118h
    mov di, MODE_INFO_OFFSET
    int 10h
    
    mov di, 0x7f00 ; ram info destination
    call get_ram_info ; get ram size (most importantly)

    mov dl, [BOOT_DRIVE]
    mov ax, KERNEL_OFFSET
    mov cx, KERNEL_SECTORS_SIZE
    call load_kernel ; read the kernel from disk

    jmp switch_to_pm ; disable interrupts, load GDT,  etc. Finally jumps to 'BEGIN_PM'

    ret

[bits 32]
begin_pm:
    call KERNEL_OFFSET ; Give control to the kernel

    jmp $


; boot magic
times 510 - ($-$$) db 0
dw 0xaa55
