[bits 16]
[org 0x7C00]
start:
    mov al, 'A'
    out 0xE9, al
    cli
    hlt
    jmp $

times 510-($-$$) db 0
dw 0xAA55
