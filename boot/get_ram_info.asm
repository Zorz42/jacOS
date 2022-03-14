
; di -> ram info destination
get_ram_info:
    pusha

    mov ebx, 0
    mov edx, 0x534D4150 ; Magic number

gather_ram_info_loop:
    mov eax, 0xE820
    mov ecx, 24
    int 0x15
    add di, 24
    cmp ebx, 0
    jne gather_ram_info_loop
    
    popa
    ret
