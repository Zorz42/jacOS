; Identical to lesson 13's boot sector, but the %included files have new paths
[org 0x7c00]
KERNEL_OFFSET equ 0x1000 ; The same one we used when linking the kernel

    mov [BOOT_DRIVE], dl ; Remember that the BIOS sets us the boot drive in 'dl' on boot
    mov bp, 0x9000
    mov sp, bp
    
    ; idk what this does
    ;mov ax, ds
    ;mov es, ax

    ;Set video mode
    mov ax, 4f02h
    mov bx, 115h
    int 10h

    ;Get video mode info
    mov ax, 4f01h
    mov cx, 115h
    mov di, modeInfo
    int 10h
    
    call load_kernel ; read the kernel from disk

    call switch_to_pm ; disable interrupts, load GDT,  etc. Finally jumps to 'BEGIN_PM'
    jmp $ ; Never executed

%include "boot/print.asm"
%include "boot/disk.asm"
%include "boot/gdt.asm"
%include "boot/switch_pm.asm"

[bits 16]
load_kernel:
    mov bx, KERNEL_OFFSET ; Read from disk and store in 0x512
    mov dh, 16 ; Our future kernel will be larger, make this big
    mov dl, [BOOT_DRIVE]
    
    call disk_load
    ret

[bits 32]
BEGIN_PM:
    mov ebx, modeInfo
    call KERNEL_OFFSET ; Give control to the kernel
    jmp $ ; Stay here when the kernel returns control to us (if ever)


BOOT_DRIVE db 0 ; It is a good idea to store it in memory because 'dl' may get overwritten
modeInfo    TIMES 256 db 0

; padding
times 510 - ($-$$) db 0
dw 0xaa55
