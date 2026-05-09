[bits 16]
[org 0x7C00]

start:
    mov al, 0x30
    out 0xE9, al
    xor ax, ax
    int 0x13
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7000
    mov [boot_drive], dl
    mov ah, 0x88
    int 0x15
    jc mem_err
    mov [0x5000], ax
    jmp detect_disk
mem_err:
    mov word [0x5000], 32768
detect_disk:
    push es
    mov ah, 0x08
    int 0x13
    pop es
    jc floppy_geom
    and cl, 0x3F
    mov [spt], cl
    mov [heads], dh
    inc byte [heads]
    jmp load_kernel
floppy_geom:
    mov byte [spt], 18
    mov byte [heads], 2
load_kernel:
    mov si, 1
    mov di, 0x8000
    mov cx, 256
.read_loop:
    push cx
    mov ax, si
    xor dx, dx
    mov bl, [spt]
    div bl
    inc ah
    mov cl, ah
    xor ah, ah
    mov bl, [heads]
    div bl
    mov dh, ah
    mov ch, al
    mov dl, [boot_drive]
    mov bx, di
    mov ax, 0x0201
    int 0x13
    jc read_error
    add di, 512
    jnc .no_wrap
    push ax
    mov ax, es
    add ax, 0x1000
    mov es, ax
    pop ax
.no_wrap:
    inc si
    pop cx
    mov al, 0x35
    out 0xE9, al
    loop .read_loop
    mov al, 0x33
    out 0xE9, al
    cli
    lgdt [gdt_desc]
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp 0x08:pmode
read_error:
    hlt
    jmp read_error
boot_drive: db 0
spt:        db 0
heads:      db 0
gdt:
    dq 0
    dw 0xFFFF, 0, 0x9A00, 0x00CF
    dw 0xFFFF, 0, 0x9200, 0x00CF
gdt_desc:
    dw gdt_desc - gdt - 1
    dd gdt
[bits 32]
pmode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000
    cld
    mov esi, 0x00008000
    mov edi, 0x00100000
    mov ecx, 131072 / 4
    rep movsd
    mov al, 0x32
    out 0xE9, al
    mov al, 0x31
    out 0xE9, al
    jmp 0x08:0x00100000

times 510-($-$$) db 0
dw 0xAA55
