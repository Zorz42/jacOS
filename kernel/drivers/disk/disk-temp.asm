[extern loadToAddress]

loadToAddress:
    push ebp
    mov ebp, esp
    mov edi, [ebp + 8]
.still_going:
    in al,dx
    test al,8               ;the sector buffer requires servicing.
    jz .still_going         ;until the sector buffer is ready.

    mov eax,512/2           ;to read 256 words = 1 sector
    xor bx,bx
    mov bl,ch               ;read CH sectors
    mul bx
    mov ecx,eax             ;RCX is counter for INSW
    mov edx,1f0h            ;Data port, in and out
    rep insw                ;in to [RDI]
    
    pop ebp
    ret
