.section .multiboot
.global _start
_start:
    jmp mb1_real

.align 8
mb2_hdr_start:
.long 0xE85250D6
.long 0
.long mb2_hdr_end - mb2_hdr_start
.long -(0xE85250D6 + 0 + (mb2_hdr_end - mb2_hdr_start))
.short 0
.short 0
.long 8
mb2_hdr_end:

.align 4
mb1_real:
    jmp mb_go

.align 4
mb1_magic:
.long 0x1BADB002
.long 0x00000003
.long -(0x1BADB002 + 0x00000003)

mb_go:

.section .bss
.align 16
stack_bottom:
.skip 32768
stack_top:

.align 4096
pml4:
.skip 4096
pdpt:
.skip 4096
pd:
.skip 4096

.section .data
.align 8
gdt64:
.quad 0
.quad 0x00AF9A000000FFFF
.quad 0x00CF92000000FFFF
gdt64_desc:
.word gdt64_desc - gdt64 - 1
.long gdt64
.long 0

.align 8
mb_magic:
.quad 0x4A4E4F42
mb_info:
.quad 0x5000

.section .text
.code32
.global start32
start32:
    movl $stack_top, %esp
    cli
    testl $0x2BADB002, %eax
    jz 1f
    movl %eax, mb_magic
    movl %ebx, mb_info
1:
    movl $__bss_start, %edi
    movl $__bss_end, %ecx
    subl %edi, %ecx
    shrl $2, %ecx
    xorl %eax, %eax
    cld
    rep stosl

    movl $pd, %edi
    movl %edi, %cr3
    movl $pml4, %edi
    movl $(pdpt + 0x3), (%edi)
    movl $(pd + 0x3), pdpt
    movl $0, %edx
2:
    movl %edx, %eax
    shll $21, %eax
    orl $0x83, %eax
    movl %eax, pd(,%edx,8)
    movl $0, pd+4(,%edx,8)
    incl %edx
    cmpl $512, %edx
    jb 2b

    movl %cr4, %eax
    orl $(1<<5), %eax
    movl %eax, %cr4
    movl $0xC0000080, %ecx
    rdmsr
    orl $(1<<8), %eax
    wrmsr
    movl %cr0, %eax
    orl $(1<<31), %eax
    movl %eax, %cr0
    lgdt gdt64_desc
    ljmp $0x08, $start64

.code64
start64:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    movq $stack_top, %rsp
    movq mb_magic, %rdi
    movq mb_info, %rsi
    xorq %rbp, %rbp
    call kmain
3:
    cli
    hlt
    jmp 3b
