[bits 32]
section .multiboot
align 4
    jmp _start
    align 4
    dd 0x1BADB002
    dd 0x00000003
    dd -(0x1BADB002 + 0x00000003)

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

section .text
global _start
extern kmain

_start:
    mov al, 0x31
    out 0xE9, al

    mov edi, 0x1000
    mov cr3, edi
    xor eax, eax
    mov ecx, 4096
    rep stosd
    mov edi, 0x1000

    mov dword [edi], 0x2003
    add edi, 0x1000
    mov dword [edi], 0x3003
    add edi, 0x1000
    mov dword [edi], 0x83
    add edi, 8
    mov dword [edi], 0x200083
    add edi, 8
    mov dword [edi], 0x400083
    add edi, 8
    mov dword [edi], 0x600083

    mov eax, cr4
    or eax, (1 << 5) | (1 << 9)
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [gdt64_desc]
    jmp 0x08:_start64

[bits 64]
_start64:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov rsp, stack_top
    call kmain
    cli
.hang:
    hlt
    jmp .hang

section .data
align 8
gdt64:
    dq 0
    dq 0x002F9A000000FFFF
    dq 0x00CF92000000FFFF
gdt64_desc:
    dw gdt64_desc - gdt64 - 1
    dd gdt64