[bits 16]
[org 0x7C00]

start:
    xor ax, ax
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
    mov ah, 0x08
    int 0x13
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
    mov ax, 0x0201
    mov bx, di
    int 0x13
    jc read_error
    add di, 512
    inc si
    pop cx
    loop .read_loop

    call enable_a20

    cli
    lgdt [gdt32_desc]
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp 0x08:pmode32

read_error:
    hlt
    jmp read_error

enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

boot_drive: db 0
spt:        db 0
heads:      db 0

align 8
gdt32:
    dq 0
    dw 0xFFFF, 0, 0x9A00, 0x00CF
    dw 0xFFFF, 0, 0x9200, 0x00CF
gdt32_desc:
    dw gdt32_desc - gdt32 - 1
    dd gdt32

[bits 32]
pmode32:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov eax, 0x9000
    mov cr3, eax

    mov dword [0x9000], 0xA000 | 3
    mov dword [0x9004], 0
    mov dword [0xA000], 0xB000 | 3
    mov dword [0xA004], 0
    mov dword [0xB000], (0 << 21) | 0x81 | 3
    mov dword [0xB004], 0

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [gdt64_desc]
    jmp 0x08:pmode64

align 8
gdt64:
    dq 0
    dq 0x00AF9A000000FFFF
    dq 0x00CF92000000FFFF
gdt64_desc:
    dw gdt64_desc - gdt64 - 1
    dd gdt64

[bits 64]
pmode64:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov rsp, 0x90000

    cld
    mov rsi, 0x8000
    mov rdi, 0x100000
    mov rcx, 131072
    rep movsb

    mov rax, 0x100000
    jmp rax

times 510-($-) db 0
dw 0xAA55
