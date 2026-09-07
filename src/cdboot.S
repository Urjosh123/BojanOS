.code16
.section .text
.global _cdstart
_cdstart:
    cli
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw $0x7C00, %sp
    sti
    movb %dl, boot_drive

    movw $0x5008, %di
    xorw %bp, %bp
e820_loop:
    movl $0x534D4150, %edx
    movl $24, %ecx
    movl $0xE820, %eax
    movl $0, 20(%di)
    int $0x13
    jc e820_done
    addw $24, %di
    incw %bp
    cmpl $0, %ebx
    jne e820_loop
e820_done:
    movw %bp, (0x5000)

    call enable_a20
    call flat_enter

    movl kern_lba, %ebp
    movl $0x100000, %ebx
    movl kern_sectors, %eax
    movl %eax, sectors_left
read_chunk:
    movl sectors_left, %ecx
    cmpl $32, %ecx
    jbe 1f
    movl $32, %ecx
1:
    movw %cx, dap_count
    movl $0x00007000, dap_buf
    movl %ebp, dap_lba
    movl $0, dap_lba + 4
    movb boot_drive, %dl
    movw $0x4200, %ax
    movw $dap, %si
    int $0x13
    jc disk_error
    call flat_refresh
    movzwl dap_count, %eax
    movl %eax, %ecx
    shll $7, %ecx
    movl $0x70000, %esi
    movl %ebx, %edi
    cld
    rep movsl
    movzwl dap_count, %edx
    addl %edx, %ebp
    movl %edx, %ecx
    shll $9, %ecx
    addl %ecx, %ebx
    subl %edx, sectors_left
    jnz read_chunk

    cli
    lgdt gdt32_desc
    movl %cr0, %eax
    orl $1, %eax
    movl %eax, %cr0
    ljmp $0x08, $pm_entry

enable_a20:
    call a20_wait_in
    movb $0xD1, %al
    outb %al, $0x64
    call a20_wait_in
    movb $0xDF, %al
    outb %al, $0x60
    call a20_wait_out
    ret
a20_wait_in:
    inb $0x64, %al
    testb $2, %al
    jnz a20_wait_in
    ret
a20_wait_out:
    inb $0x64, %al
    testb $1, %al
    jz a20_wait_out
    inb $0x60, %al
    ret

flat_enter:
    movl %eax, save_eax
    cli
    lgdt gdt32_desc
    movl %cr0, %eax
    orl $1, %eax
    movl %eax, %cr0
    ljmp $0x08, $flat_pm

flat_refresh:
    cli
    movl %cr0, %eax
    orl $1, %eax
    movl %eax, %cr0
    jmp refresh_pm16

.code32
flat_pm:
    movw $0x10, %bx
    movw %bx, %ds
    movw %bx, %es
    movw %bx, %fs
    movw %bx, %gs
    movl %cr0, %eax
    andl $0xFFFFFFFE, %eax
    movl %eax, %cr0
    ljmp $0x00, $flat_rm

refresh_pm:
    movw $0x10, %bx
    movw %bx, %ds
    movw %bx, %es
    movw %bx, %fs
    movw %bx, %gs
    movl %cr0, %eax
    andl $0xFFFFFFFE, %eax
    movl %eax, %cr0
    ljmp $0x00, $refresh_rm

.code16
refresh_pm16:
    jmpl $0x08, $refresh_pm

.code16
flat_rm:
    movl save_eax, %eax
    sti
    ret

refresh_rm:
    sti
    ret

disk_error:
    movw $errmsg, %si
2:
    lodsb
    testb %al, %al
    jz 3f
    movb $0x0E, %ah
    movb $0x07, %bl
    int $0x10
    jmp 2b
3:
    hlt
    jmp 3b

.code32
pm_entry:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    movl $0x7C00, %esp
    movl $0x100000, %eax
    jmpl *%eax

.code16
.section .rodata
.align 8
gdt32:
    .quad 0
    .quad 0x00CF9A000000FFFF
    .quad 0x00CF92000000FFFF
gdt32_desc:
    .word gdt32_desc - gdt32 - 1
    .long gdt32

errmsg:
    .asciz "BojanOS: disk read error"

.section .data
.align 4
dap:
    .byte 0x10
    .byte 0
dap_count:
    .word 0
dap_buf:
    .long 0
dap_lba:
    .quad 0

boot_drive:
    .byte 0

.align 4
sectors_left:
    .long 0

save_eax:
    .long 0

.align 4
boot_marker:
    .ascii "BOJANBOOT"
kern_lba:
    .long 0
kern_sectors:
    .long 0
