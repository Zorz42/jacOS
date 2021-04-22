;=============================================================================
; ATA read sectors (CHS mode)
; Max head index is 15, giving 16 possible heads
; Max cylinder index can be a very large number (up to 65535)
; Sector is usually always 1-63, sector 0 reserved, max 255 sectors/track
; If using 63 sectors/track, max disk size = 31.5GB
; If using 255 sectors/track, max disk size = 127.5GB
; See OSDev forum links in bottom of [http://wiki.osdev.org/ATA]
;
; @param EBX The CHS values; 2 bytes, 1 byte (BH), 1 byte (BL) accordingly
; @param CH The number of sectors to read
; @param RDI The address of buffer to put data obtained from disk
;
; @return None
;=============================================================================

[extern ata_chs_read]

ata_chs_read:
                mov edi, 0x10000000
 
.still_going:   in al,dx
                test al,8               ;the sector buffer requires servicing.
                jz .still_going         ;until the sector buffer is ready.
 
                mov eax,512/2           ;to read 256 words = 1 sector
                xor bx,bx
                mov bl,ch               ;read CH sectors
                mul bx
                mov ecx,eax             ;RCX is counter for INSW
                mov edx,1f0h            ;Data port, in and out
                rep insw                ;in to [RDI]
 
                ret
