[bits 64]
section .text
global idt_install
global idt_set_gate
global isr_stubs
global idt_load

extern isr_handler

idt_install:
    lidt [idt_desc]
    ret

idt_load:
    lidt [idt_desc]
    ret

idt_set_gate:
    push rbx
    mov rbx, rdi
    shl rbx, 4
    lea rax, [idt + rbx]
    mov rbx, rsi
    mov [rax], bx
    shr rbx, 16
    mov [rax + 6], bx
    shr rbx, 16
    mov [rax + 8], ebx
    mov word [rax + 2], 0x08
    mov byte [rax + 4], 0
    mov byte [rax + 5], 0x8E
    pop rbx
    ret

%assign i 0
%rep 32
isr_stub_%+i:
    push qword 0
    push qword i
    jmp isr_common
%assign i i+1
%endrep

isr_common:
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    xor rax, rax
    mov ax, ds
    push rax
    xor rax, rax
    mov ax, es
    push rax
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov rdi, rsp
    call isr_handler

    pop gs
    pop fs
    pop rax
    mov es, ax
    pop rax
    mov ds, ax
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax
    add rsp, 16
    iretq

section .data
align 16
idt:
    times 256 * 16 db 0
isr_stubs:
%assign i 0
%rep 32
    dq isr_stub_%+i
%assign i i+1
%endrep
idt_desc:
    dw 256 * 16 - 1
    dq idt
