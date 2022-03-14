[bits 16]

KERNEL_LOAD_SECTORS_AT_A_TIME equ 50 ; How much kernel should be loaded at a time
KERNEL_TEMP_LOAD_POS equ 0x1000 ; Where should the temporary kernel buffer be located

; dh -> sector count
; dl -> drive number
; cx -> LBA sector index
; es -> destination segment
; bx -> destination address

[bits 16]
disk_load:
    pusha

    mov ah, 0x02 ; ah <- int 0x13 function. 0x02 = 'read'
    mov al, dh   ; al <- number of sectors to read (0x01 .. 0x80)

    push ax
    push dx ; save dl -> drive number

    mov dx, 0
    mov ax, cx
    mov cx, 36 ; sectors per cylinder
    div cx

    mov cl, dl ; cl -> sector
    inc cl ; because sector 0 doesnt exist

    mov ch, al ; ch -> cylinder

    pop dx
    pop ax

    mov dh, ch ; dh <- head number (0x0 .. 0xF)
    and dh, 1
    shr ch, 1

    ; [es:bx] <- pointer to buffer where the data will be stored
    ; caller sets it up for us, and it is actually the standard location for int 13h
    int 0x13      ; BIOS interrupt

    popa
    ret


; fs -> from
; gs -> to
; cs -> length

copy_memory:
    pusha

mov bx, 0
    copy_loop:
    mov ax, [fs:bx]
    mov [gs:bx], ax
    add bx, 2
    cmp bx, cx
    jl copy_loop

    popa
    ret



; dl -> drive number
; ax -> address in memory to load
; dx -> size of kernel (in sectors)

load_kernel:
    pusha

    shr ax, 4
    
    push cx
    push ax

    mov ax, 0

load_loop:
    mov cx, ax
    inc cx
    mov bx, KERNEL_TEMP_LOAD_POS
    mov dh, KERNEL_LOAD_SECTORS_AT_A_TIME

    call disk_load
    
    pop cx
    mov bx, ax
    shl bx, 5
    add bx, cx
    mov gs, bx
    push cx

    mov bx, KERNEL_TEMP_LOAD_POS / 0x10
    mov fs, bx

    mov cx, KERNEL_LOAD_SECTORS_AT_A_TIME * 512

    call copy_memory
 
    add ax, KERNEL_LOAD_SECTORS_AT_A_TIME
    
    pop bx
    pop cx
    cmp ax, cx
    push cx
    push bx
 
    jl load_loop

    pop ax
    pop cx
 
    popa
    ret
