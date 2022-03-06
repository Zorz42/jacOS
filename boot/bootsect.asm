[org 0x7c00]
KERNEL_OFFSET equ 0x1000 ; The same one we used when linking the kernel
MODE_INFO_OFFSET equ 0x7d00 ; Where the vesa mode info will be saved
RAM_INFO_OFFSET equ 0x7e00 ; Where the ram info will be saved


mov [BOOT_DRIVE], dl
mov bp, 0x9000
mov sp, bp

mov ax, ds
mov es, ax

; Set video mode
mov ax, 4f02h
mov bx, 118h
int 10h

; Get video mode info
mov ax, 4f01h
mov cx, 118h
mov di, MODE_INFO_OFFSET
int 10h


call get_ram_info

call load_kernel ; read the kernel from disk

call switch_to_pm ; disable interrupts, load GDT,  etc. Finally jumps to 'BEGIN_PM'
jmp $ ; Never executed


%include "boot/print.asm"
%include "boot/disk.asm"
%include "boot/gdt.asm"
%include "boot/switch_pm.asm"
%include "boot/kernel_size.asm"

[bits 16]
load_kernel:
    mov bx, KERNEL_OFFSET
    mov dh, KERNEL_SECTORS_SIZE
    mov dl, [BOOT_DRIVE]
    
    call disk_load
    ret

get_ram_info:
    pusha
    mov di, RAM_INFO_OFFSET
    
    mov ebx, 0
    mov edx, 0x534D4150
    
gather_ram_entry:
    mov eax, 0xE820
    mov ecx, 24
    int 0x15
    cmp ebx, 0
    je get_ram_info_end
    add di, 24
    jmp gather_ram_entry
    
get_ram_info_end:
    popa
    ret


[bits 32]
BEGIN_PM:
    call KERNEL_OFFSET ; Give control to the kernel
    
    jmp $
    

BOOT_DRIVE db 0

; padding
times 510 - ($-$$) db 0
dw 0xaa55
