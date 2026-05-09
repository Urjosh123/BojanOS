	.text
	.file	"vga.c"
	.file	1 "C:/Users/Gower" "AppData\\Local\\Temp\\zig_dir\\zig-windows-x86_64-0.13.0\\lib\\include/__stddef_size_t.h"
	.file	2 "C:\\Users\\Gower\\Downloads\\BojanOS\\include" "vga.h"
	.file	3 "C:/Users/Gower" "AppData\\Local\\Temp\\zig_dir\\zig-windows-x86_64-0.13.0\\lib\\include/stdint.h"
	.file	4 "C:\\Users\\Gower\\Downloads\\BojanOS\\src" "vga.c"
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function vga_init
.LCPI0_0:
	.short	1824                            # 0x720
	.short	1824                            # 0x720
	.short	1824                            # 0x720
	.short	1824                            # 0x720
	.short	1824                            # 0x720
	.short	1824                            # 0x720
	.short	1824                            # 0x720
	.short	1824                            # 0x720
	.text
	.globl	vga_init
	.p2align	4, 0x90
	.type	vga_init,@function
vga_init:                               # @vga_init
.Lfunc_begin0:
	.loc	4 13 0                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:13:0
	.cfi_sections .debug_frame
	.cfi_startproc
# %bb.0:
	movl	$753808, %eax                   # imm = 0xB8090
.Ltmp0:
	.loc	4 16 11 prologue_end            # C:\Users\Gower\Downloads\BojanOS\src\vga.c:16:11
	movb	$7, color(%rip)
	xorl	%ecx, %ecx
	#DEBUG_VALUE: y <- 0
	movaps	.LCPI0_0(%rip), %xmm0           # xmm0 = [1824,1824,1824,1824,1824,1824,1824,1824]
	.p2align	4, 0x90
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
.Ltmp1:
	#DEBUG_VALUE: y <- [DW_OP_consts 160, DW_OP_div, DW_OP_stack_value] $rcx
	#DEBUG_VALUE: x <- 0
	.loc	4 32 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:32:43
	movaps	%xmm0, -144(%rax,%rcx)
	movaps	%xmm0, -128(%rax,%rcx)
	movaps	%xmm0, -112(%rax,%rcx)
	movaps	%xmm0, -96(%rax,%rcx)
	movaps	%xmm0, -80(%rax,%rcx)
	movaps	%xmm0, -64(%rax,%rcx)
	movaps	%xmm0, -48(%rax,%rcx)
	movaps	%xmm0, -32(%rax,%rcx)
	movaps	%xmm0, -16(%rax,%rcx)
	movaps	%xmm0, (%rax,%rcx)
.Ltmp2:
	#DEBUG_VALUE: y <- [DW_OP_consts 160, DW_OP_div, DW_OP_consts 1, DW_OP_plus, DW_OP_stack_value] $rcx
	.loc	4 30 26                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:30:26
	addq	$160, %rcx
.Ltmp3:
	cmpq	$4000, %rcx                     # imm = 0xFA0
.Ltmp4:
	.loc	4 30 5 is_stmt 0                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:30:5
	jne	.LBB0_1
.Ltmp5:
# %bb.2:
	.loc	4 33 9 is_stmt 1                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:33:9
	movq	$0, row(%rip)
	.loc	4 34 9                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:34:9
	movq	$0, col(%rip)
	.loc	4 35 5                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:35:5
	movb	$70, %al
	#APP
	outb	%al, $233
	#NO_APP
.Ltmp6:
	.loc	4 18 1                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:18:1
	retq
.Ltmp7:
.Lfunc_end0:
	.size	vga_init, .Lfunc_end0-vga_init
	.cfi_endproc
                                        # -- End function
	.globl	vga_clear                       # -- Begin function vga_clear
	.p2align	4, 0x90
	.type	vga_clear,@function
vga_clear:                              # @vga_clear
.Lfunc_begin1:
	.loc	4 29 0                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:29:0
	.cfi_startproc
# %bb.0:
	#DEBUG_VALUE: y <- 0
	movl	$753808, %eax                   # imm = 0xB8090
	movzbl	color(%rip), %ecx
	shll	$8, %ecx
	orl	$32, %ecx
.Ltmp8:
	.loc	4 31 9 prologue_end             # C:\Users\Gower\Downloads\BojanOS\src\vga.c:31:9
	movd	%ecx, %xmm0
	pshuflw	$0, %xmm0, %xmm0                # xmm0 = xmm0[0,0,0,0,4,5,6,7]
	pshufd	$0, %xmm0, %xmm0                # xmm0 = xmm0[0,0,0,0]
	xorl	%ecx, %ecx
.Ltmp9:
	.p2align	4, 0x90
.LBB1_1:                                # =>This Inner Loop Header: Depth=1
	#DEBUG_VALUE: y <- [DW_OP_consts 160, DW_OP_div, DW_OP_stack_value] $rcx
	#DEBUG_VALUE: x <- 0
	.loc	4 32 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:32:43
	movdqa	%xmm0, -144(%rax,%rcx)
	movdqa	%xmm0, -128(%rax,%rcx)
	movdqa	%xmm0, -112(%rax,%rcx)
	movdqa	%xmm0, -96(%rax,%rcx)
	movdqa	%xmm0, -80(%rax,%rcx)
	movdqa	%xmm0, -64(%rax,%rcx)
	movdqa	%xmm0, -48(%rax,%rcx)
	movdqa	%xmm0, -32(%rax,%rcx)
	movdqa	%xmm0, -16(%rax,%rcx)
	movdqa	%xmm0, (%rax,%rcx)
.Ltmp10:
	#DEBUG_VALUE: y <- [DW_OP_consts 160, DW_OP_div, DW_OP_consts 1, DW_OP_plus, DW_OP_stack_value] $rcx
	.loc	4 30 26                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:30:26
	addq	$160, %rcx
.Ltmp11:
	cmpq	$4000, %rcx                     # imm = 0xFA0
.Ltmp12:
	.loc	4 30 5 is_stmt 0                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:30:5
	jne	.LBB1_1
.Ltmp13:
# %bb.2:
	.loc	4 33 9 is_stmt 1                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:33:9
	movq	$0, row(%rip)
	.loc	4 34 9                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:34:9
	movq	$0, col(%rip)
	.loc	4 35 5                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:35:5
	movb	$70, %al
	#APP
	outb	%al, $233
	#NO_APP
	.loc	4 36 1                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:36:1
	retq
.Ltmp14:
.Lfunc_end1:
	.size	vga_clear, .Lfunc_end1-vga_clear
	.cfi_endproc
                                        # -- End function
	.globl	vga_setcolor                    # -- Begin function vga_setcolor
	.p2align	4, 0x90
	.type	vga_setcolor,@function
vga_setcolor:                           # @vga_setcolor
.Lfunc_begin2:
	.cfi_startproc
# %bb.0:
	#DEBUG_VALUE: vga_setcolor:c <- $edi
	.loc	4 20 11 prologue_end            # C:\Users\Gower\Downloads\BojanOS\src\vga.c:20:11
	movb	%dil, color(%rip)
	.loc	4 21 1                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:21:1
	retq
.Ltmp15:
.Lfunc_end2:
	.size	vga_setcolor, .Lfunc_end2-vga_setcolor
	.cfi_endproc
                                        # -- End function
	.globl	vga_setcursor                   # -- Begin function vga_setcursor
	.p2align	4, 0x90
	.type	vga_setcursor,@function
vga_setcursor:                          # @vga_setcursor
.Lfunc_begin3:
	.cfi_startproc
# %bb.0:
	#DEBUG_VALUE: vga_setcursor:x <- $rdi
	#DEBUG_VALUE: vga_setcursor:y <- $rsi
	.loc	4 23 22 prologue_end            # C:\Users\Gower\Downloads\BojanOS\src\vga.c:23:22
	leaq	(%rsi,%rsi,4), %rcx
	shlq	$4, %rcx
	.loc	4 23 34 is_stmt 0               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:23:34
	addq	%rdi, %rcx
.Ltmp16:
	#DEBUG_VALUE: vga_setcursor:pos <- [DW_OP_LLVM_convert 64 7, DW_OP_LLVM_convert 16 7, DW_OP_stack_value] $rcx
	#DEBUG_VALUE: outb:port <- 980
	#DEBUG_VALUE: outb:val <- 14
	.file	5 "C:\\Users\\Gower\\Downloads\\BojanOS\\include" "io.h"
	.loc	5 5 5 is_stmt 1                 # C:\Users\Gower\Downloads\BojanOS\include/io.h:5:5
	movb	$14, %al
	movw	$980, %dx                       # imm = 0x3D4
	#APP
	outb	%al, %dx
	#NO_APP
.Ltmp17:
	.loc	4 25 17                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:25:17
	movl	%ecx, %eax
	shrl	$8, %eax
.Ltmp18:
	#DEBUG_VALUE: outb:val <- $al
	#DEBUG_VALUE: outb:port <- 981
                                        # kill: def $al killed $al killed $eax
	.loc	5 5 5                           # C:\Users\Gower\Downloads\BojanOS\include/io.h:5:5
	movw	$981, %dx                       # imm = 0x3D5
	#APP
	outb	%al, %dx
	#NO_APP
.Ltmp19:
	#DEBUG_VALUE: outb:port <- 980
	#DEBUG_VALUE: outb:val <- 15
	.loc	5 5 5 is_stmt 0                 # C:\Users\Gower\Downloads\BojanOS\include/io.h:5:5
	movb	$15, %al
.Ltmp20:
	movw	$980, %dx                       # imm = 0x3D4
	#APP
	outb	%al, %dx
	#NO_APP
.Ltmp21:
	#DEBUG_VALUE: outb:port <- 981
	#DEBUG_VALUE: outb:val <- $cl
	.loc	5 5 5                           # C:\Users\Gower\Downloads\BojanOS\include/io.h:5:5
	movl	%ecx, %eax
	movw	$981, %dx                       # imm = 0x3D5
	#APP
	outb	%al, %dx
	#NO_APP
.Ltmp22:
	.loc	4 28 1 is_stmt 1                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:28:1
	retq
.Ltmp23:
.Lfunc_end3:
	.size	vga_setcursor, .Lfunc_end3-vga_setcursor
	.cfi_endproc
                                        # -- End function
	.globl	vga_scroll                      # -- Begin function vga_scroll
	.p2align	4, 0x90
	.type	vga_scroll,@function
vga_scroll:                             # @vga_scroll
.Lfunc_begin4:
	.loc	4 37 0                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:37:0
	.cfi_startproc
# %bb.0:
	movl	$753968, %eax                   # imm = 0xB8130
	xorl	%ecx, %ecx
	#DEBUG_VALUE: y <- 0
	.p2align	4, 0x90
.LBB4_1:                                # =>This Inner Loop Header: Depth=1
.Ltmp24:
	#DEBUG_VALUE: y <- [DW_OP_consts 160, DW_OP_div, DW_OP_stack_value] $rcx
	#DEBUG_VALUE: x <- 0
	.loc	4 40 45 prologue_end            # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:45
	movaps	-144(%rax,%rcx), %xmm0
	movaps	-128(%rax,%rcx), %xmm1
	movaps	-112(%rax,%rcx), %xmm2
	movaps	-96(%rax,%rcx), %xmm3
	.loc	4 40 43 is_stmt 0               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:43
	movaps	%xmm0, -304(%rax,%rcx)
	movaps	%xmm1, -288(%rax,%rcx)
	movaps	%xmm2, -272(%rax,%rcx)
	movaps	%xmm3, -256(%rax,%rcx)
	.loc	4 40 45                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:45
	movaps	-80(%rax,%rcx), %xmm0
	.loc	4 40 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:43
	movaps	%xmm0, -240(%rax,%rcx)
	.loc	4 40 45                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:45
	movaps	-64(%rax,%rcx), %xmm0
	.loc	4 40 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:43
	movaps	%xmm0, -224(%rax,%rcx)
	.loc	4 40 45                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:45
	movaps	-48(%rax,%rcx), %xmm0
	.loc	4 40 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:43
	movaps	%xmm0, -208(%rax,%rcx)
	.loc	4 40 45                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:45
	movaps	-32(%rax,%rcx), %xmm0
	.loc	4 40 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:43
	movaps	%xmm0, -192(%rax,%rcx)
	.loc	4 40 45                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:45
	movaps	-16(%rax,%rcx), %xmm0
	.loc	4 40 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:43
	movaps	%xmm0, -176(%rax,%rcx)
	.loc	4 40 45                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:45
	movaps	(%rax,%rcx), %xmm0
	.loc	4 40 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:43
	movaps	%xmm0, -160(%rax,%rcx)
.Ltmp25:
	#DEBUG_VALUE: y <- [DW_OP_consts 160, DW_OP_div, DW_OP_consts 1, DW_OP_plus, DW_OP_stack_value] $rcx
	.loc	4 38 26 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:38:26
	addq	$160, %rcx
.Ltmp26:
	cmpq	$3840, %rcx                     # imm = 0xF00
.Ltmp27:
	.loc	4 38 5 is_stmt 0                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:38:5
	jne	.LBB4_1
.Ltmp28:
# %bb.2:
	#DEBUG_VALUE: x <- 0
	.loc	4 0 5                           # C:\Users\Gower\Downloads\BojanOS\src\vga.c:0:5
	movzbl	color(%rip), %eax
	shll	$8, %eax
	orl	$32, %eax
.Ltmp29:
	.loc	4 41 5 is_stmt 1                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:41:5
	movd	%eax, %xmm0
	pshuflw	$0, %xmm0, %xmm0                # xmm0 = xmm0[0,0,0,0,4,5,6,7]
	pshufd	$0, %xmm0, %xmm0                # xmm0 = xmm0[0,0,0,0]
.Ltmp30:
	.loc	4 42 54                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:42:54
	movdqa	%xmm0, 757504
	movdqa	%xmm0, 757520
	movdqa	%xmm0, 757536
	movdqa	%xmm0, 757552
	movdqa	%xmm0, 757568
	movdqa	%xmm0, 757584
	movdqa	%xmm0, 757600
	movdqa	%xmm0, 757616
	movdqa	%xmm0, 757632
	movdqa	%xmm0, 757648
.Ltmp31:
	.loc	4 43 9                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:43:9
	movq	$24, row(%rip)
	.loc	4 44 9                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:44:9
	movq	$0, col(%rip)
	.loc	4 45 1                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:45:1
	retq
.Ltmp32:
.Lfunc_end4:
	.size	vga_scroll, .Lfunc_end4-vga_scroll
	.cfi_endproc
                                        # -- End function
	.globl	vga_backspace                   # -- Begin function vga_backspace
	.p2align	4, 0x90
	.type	vga_backspace,@function
vga_backspace:                          # @vga_backspace
.Lfunc_begin5:
	.cfi_startproc
# %bb.0:
	.loc	4 47 9 prologue_end             # C:\Users\Gower\Downloads\BojanOS\src\vga.c:47:9
	movq	col(%rip), %rax
	.loc	4 47 13 is_stmt 0               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:47:13
	testq	%rax, %rax
.Ltmp33:
	.loc	4 47 9                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:47:9
	je	.LBB5_2
# %bb.1:
.Ltmp34:
	.loc	4 48 12 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:48:12
	leaq	-1(%rax), %rcx
	movq	%rcx, col(%rip)
.Ltmp35:
	#DEBUG_VALUE: vga_entry:c <- 32
	#DEBUG_VALUE: vga_entry:color <- undef
	.loc	4 11 26                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:11:26
	movzbl	color(%rip), %ecx
	.loc	4 11 42 is_stmt 0               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:11:42
	shll	$8, %ecx
	.loc	4 11 24                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:11:24
	orl	$32, %ecx
.Ltmp36:
	.loc	4 49 20 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:49:20
	movq	row(%rip), %rdx
	.loc	4 49 24 is_stmt 0               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:49:24
	shlq	$4, %rdx
	leaq	(%rdx,%rdx,4), %rdx
	.loc	4 49 9                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:49:9
	leaq	-2(,%rax,2), %rsi
	.loc	4 49 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:49:43
	movw	%cx, 753664(%rsi,%rdx,2)
.Ltmp37:
	#DEBUG_VALUE: vga_setcursor:x <- undef
	#DEBUG_VALUE: vga_setcursor:y <- undef
	.loc	4 23 34 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:23:34
	leaq	(%rax,%rdx), %rcx
	addq	$-1, %rcx
.Ltmp38:
	#DEBUG_VALUE: vga_setcursor:pos <- [DW_OP_LLVM_convert 64 7, DW_OP_LLVM_convert 16 7, DW_OP_stack_value] $rcx
	#DEBUG_VALUE: outb:port <- 980
	#DEBUG_VALUE: outb:val <- 14
	.loc	5 5 5                           # C:\Users\Gower\Downloads\BojanOS\include/io.h:5:5
	movb	$14, %al
	movw	$980, %dx                       # imm = 0x3D4
	#APP
	outb	%al, %dx
	#NO_APP
.Ltmp39:
	.loc	4 25 17                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:25:17
	movl	%ecx, %eax
	shrl	$8, %eax
.Ltmp40:
	#DEBUG_VALUE: outb:val <- $al
	#DEBUG_VALUE: outb:port <- 981
                                        # kill: def $al killed $al killed $eax
	.loc	5 5 5                           # C:\Users\Gower\Downloads\BojanOS\include/io.h:5:5
	movw	$981, %dx                       # imm = 0x3D5
	#APP
	outb	%al, %dx
	#NO_APP
.Ltmp41:
	#DEBUG_VALUE: outb:port <- 980
	#DEBUG_VALUE: outb:val <- 15
	.loc	5 5 5 is_stmt 0                 # C:\Users\Gower\Downloads\BojanOS\include/io.h:5:5
	movb	$15, %al
.Ltmp42:
	movw	$980, %dx                       # imm = 0x3D4
	#APP
	outb	%al, %dx
	#NO_APP
.Ltmp43:
	#DEBUG_VALUE: outb:port <- 981
	#DEBUG_VALUE: outb:val <- $cl
	.loc	5 5 5                           # C:\Users\Gower\Downloads\BojanOS\include/io.h:5:5
	movl	%ecx, %eax
	movw	$981, %dx                       # imm = 0x3D5
	#APP
	outb	%al, %dx
	#NO_APP
.Ltmp44:
.LBB5_2:
	.loc	4 52 1 is_stmt 1                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:52:1
	retq
.Ltmp45:
.Lfunc_end5:
	.size	vga_backspace, .Lfunc_end5-vga_backspace
	.cfi_endproc
                                        # -- End function
	.globl	vga_putchar                     # -- Begin function vga_putchar
	.p2align	4, 0x90
	.type	vga_putchar,@function
vga_putchar:                            # @vga_putchar
.Lfunc_begin6:
	.loc	4 53 0                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:53:0
	.cfi_startproc
# %bb.0:
	#DEBUG_VALUE: vga_putchar:c <- $edi
	movl	$753968, %eax                   # imm = 0xB8130
.Ltmp46:
	.loc	4 54 9 prologue_end             # C:\Users\Gower\Downloads\BojanOS\src\vga.c:54:9
	cmpl	$9, %edi
	je	.LBB6_3
.Ltmp47:
# %bb.1:
	#DEBUG_VALUE: vga_putchar:c <- $edi
	cmpl	$10, %edi
	je	.LBB6_2
.Ltmp48:
# %bb.10:
	#DEBUG_VALUE: vga_putchar:c <- $edi
	#DEBUG_VALUE: vga_entry:c <- $dil
	#DEBUG_VALUE: vga_entry:color <- undef
	.loc	4 11 12                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:11:12
	movzbl	%dil, %ecx
	.loc	4 11 26 is_stmt 0               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:11:26
	movzbl	color(%rip), %esi
	.loc	4 11 42                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:11:42
	shll	$8, %esi
	.loc	4 11 24                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:11:24
	orl	%ecx, %esi
.Ltmp49:
	.loc	4 64 20 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:64:20
	movq	row(%rip), %rcx
	.loc	4 64 38 is_stmt 0               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:64:38
	movq	col(%rip), %rdx
	.loc	4 64 9                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:64:9
	leaq	(%rcx,%rcx,4), %rcx
	shlq	$5, %rcx
	addq	%rax, %rcx
	.loc	4 64 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:64:43
	movw	%si, -304(%rcx,%rdx,2)
	.loc	4 65 12 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:65:12
	addq	$1, %rdx
	movq	%rdx, %rcx
	jmp	.LBB6_11
.Ltmp50:
.LBB6_3:
	#DEBUG_VALUE: vga_putchar:c <- $edi
	.loc	4 58 24                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:58:24
	movq	col(%rip), %rdx
	.loc	4 58 33 is_stmt 0               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:58:33
	movq	%rdx, %rsi
	andq	$-4, %rsi
	addq	$4, %rsi
.Ltmp51:
	#DEBUG_VALUE: next <- $rsi
	.loc	4 59 9 is_stmt 1                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:59:9
	cmpq	$80, %rsi
	movl	$80, %ecx
	cmovbq	%rsi, %rcx
	.loc	4 59 27 is_stmt 0               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:59:27
	movq	%rcx, %rdi
.Ltmp52:
	#DEBUG_VALUE: vga_putchar:c <- [DW_OP_LLVM_entry_value 1] $edi
	subq	%rdx, %rdi
	.loc	4 59 9                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:59:9
	jbe	.LBB6_12
.Ltmp53:
# %bb.4:
	#DEBUG_VALUE: vga_putchar:c <- [DW_OP_LLVM_entry_value 1] $edi
	#DEBUG_VALUE: next <- $rsi
	.loc	4 0 9                           # C:\Users\Gower\Downloads\BojanOS\src\vga.c:0:9
	movzbl	color(%rip), %esi
.Ltmp54:
	shll	$8, %esi
	orl	$32, %esi
	movq	row(%rip), %r8
	leaq	(%r8,%r8,4), %r8
	shlq	$5, %r8
	.loc	4 59 9                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:59:9
	cmpq	$16, %rdi
	jb	.LBB6_8
.Ltmp55:
# %bb.5:
	#DEBUG_VALUE: vga_putchar:c <- [DW_OP_LLVM_entry_value 1] $edi
	movq	%rdi, %r9
	andq	$-16, %r9
	movd	%esi, %xmm0
	pshuflw	$0, %xmm0, %xmm0                # xmm0 = xmm0[0,0,0,0,4,5,6,7]
	pshufd	$0, %xmm0, %xmm0                # xmm0 = xmm0[0,0,0,0]
	leaq	(%r8,%rdx,2), %r10
	addq	%r9, %rdx
	addq	%rax, %r10
	addq	$-288, %r10                     # imm = 0xFEE0
	xorl	%r11d, %r11d
.Ltmp56:
	.p2align	4, 0x90
.LBB6_6:                                # =>This Inner Loop Header: Depth=1
	#DEBUG_VALUE: vga_putchar:c <- [DW_OP_LLVM_entry_value 1] $edi
	.loc	4 60 47 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:60:47
	movdqu	%xmm0, -16(%r10,%r11,2)
	movdqu	%xmm0, (%r10,%r11,2)
	addq	$16, %r11
	cmpq	%r11, %r9
	jne	.LBB6_6
.Ltmp57:
# %bb.7:
	#DEBUG_VALUE: vga_putchar:c <- [DW_OP_LLVM_entry_value 1] $edi
	.loc	4 59 9                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:59:9
	cmpq	%r9, %rdi
	je	.LBB6_11
.Ltmp58:
.LBB6_8:
	#DEBUG_VALUE: vga_putchar:c <- [DW_OP_LLVM_entry_value 1] $edi
	.loc	4 0 9 is_stmt 0                 # C:\Users\Gower\Downloads\BojanOS\src\vga.c:0:9
	leaq	(%rax,%r8), %rdi
	addq	$-304, %rdi                     # imm = 0xFED0
.Ltmp59:
	.p2align	4, 0x90
.LBB6_9:                                # =>This Inner Loop Header: Depth=1
	#DEBUG_VALUE: vga_putchar:c <- [DW_OP_LLVM_entry_value 1] $edi
	#DEBUG_VALUE: vga_entry:c <- 32
	.loc	4 60 47 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:60:47
	movw	%si, (%rdi,%rdx,2)
	.loc	4 61 16                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:61:16
	addq	$1, %rdx
.Ltmp60:
	.loc	4 59 27                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:59:27
	cmpq	%rdx, %rcx
.Ltmp61:
	#DEBUG_VALUE: vga_entry:color <- undef
	.loc	4 59 9 is_stmt 0                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:59:9
	jne	.LBB6_9
.Ltmp62:
.LBB6_11:
	#DEBUG_VALUE: vga_putchar:c <- [DW_OP_LLVM_entry_value 1] $edi
	.loc	4 0 0                           # C:\Users\Gower\Downloads\BojanOS\src\vga.c:0:0
	movq	%rdx, col(%rip)
	movq	%rcx, %rdx
.Ltmp63:
.LBB6_12:
	#DEBUG_VALUE: vga_putchar:c <- [DW_OP_LLVM_entry_value 1] $edi
	.loc	4 67 13 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:67:13
	cmpq	$80, %rdx
.Ltmp64:
	.loc	4 67 9 is_stmt 0                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:67:9
	jb	.LBB6_13
.Ltmp65:
.LBB6_2:
	#DEBUG_VALUE: vga_putchar:c <- [DW_OP_LLVM_entry_value 1] $edi
	.loc	4 0 0                           # C:\Users\Gower\Downloads\BojanOS\src\vga.c:0:0
	movq	$0, col(%rip)
	movq	row(%rip), %rcx
	addq	$1, %rcx
	movq	%rcx, row(%rip)
	xorl	%edx, %edx
.Ltmp66:
	.loc	4 71 13 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:71:13
	cmpq	$25, %rcx
.Ltmp67:
	.loc	4 71 9 is_stmt 0                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:71:9
	jae	.LBB6_15
	jmp	.LBB6_18
.Ltmp68:
.LBB6_13:
	#DEBUG_VALUE: vga_putchar:c <- [DW_OP_LLVM_entry_value 1] $edi
	.loc	4 71 9                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:71:9
	movq	row(%rip), %rcx
	.loc	4 71 13                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:71:13
	cmpq	$25, %rcx
.Ltmp69:
	.loc	4 71 9                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:71:9
	jb	.LBB6_18
.Ltmp70:
.LBB6_15:
	#DEBUG_VALUE: vga_putchar:c <- [DW_OP_LLVM_entry_value 1] $edi
	.loc	4 0 9                           # C:\Users\Gower\Downloads\BojanOS\src\vga.c:0:9
	xorl	%ecx, %ecx
.Ltmp71:
	.p2align	4, 0x90
.LBB6_16:                               # =>This Inner Loop Header: Depth=1
	#DEBUG_VALUE: vga_putchar:c <- [DW_OP_LLVM_entry_value 1] $edi
	#DEBUG_VALUE: y <- [DW_OP_consts 160, DW_OP_div, DW_OP_stack_value] $rcx
	#DEBUG_VALUE: x <- 0
	.loc	4 40 45 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:45
	movaps	-144(%rax,%rcx), %xmm0
	movaps	-128(%rax,%rcx), %xmm1
	movaps	-112(%rax,%rcx), %xmm2
	movaps	-96(%rax,%rcx), %xmm3
	.loc	4 40 43 is_stmt 0               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:43
	movaps	%xmm0, -304(%rax,%rcx)
	movaps	%xmm1, -288(%rax,%rcx)
	movaps	%xmm2, -272(%rax,%rcx)
	movaps	%xmm3, -256(%rax,%rcx)
	.loc	4 40 45                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:45
	movaps	-80(%rax,%rcx), %xmm0
	.loc	4 40 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:43
	movaps	%xmm0, -240(%rax,%rcx)
	.loc	4 40 45                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:45
	movaps	-64(%rax,%rcx), %xmm0
	.loc	4 40 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:43
	movaps	%xmm0, -224(%rax,%rcx)
	.loc	4 40 45                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:45
	movaps	-48(%rax,%rcx), %xmm0
	.loc	4 40 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:43
	movaps	%xmm0, -208(%rax,%rcx)
	.loc	4 40 45                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:45
	movaps	-32(%rax,%rcx), %xmm0
	.loc	4 40 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:43
	movaps	%xmm0, -192(%rax,%rcx)
	.loc	4 40 45                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:45
	movaps	-16(%rax,%rcx), %xmm0
	.loc	4 40 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:43
	movaps	%xmm0, -176(%rax,%rcx)
	.loc	4 40 45                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:45
	movdqa	(%rax,%rcx), %xmm0
	.loc	4 40 43                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:40:43
	movdqa	%xmm0, -160(%rax,%rcx)
.Ltmp72:
	#DEBUG_VALUE: y <- [DW_OP_consts 160, DW_OP_div, DW_OP_consts 1, DW_OP_plus, DW_OP_stack_value] $rcx
	.loc	4 38 26 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:38:26
	addq	$160, %rcx
.Ltmp73:
	cmpq	$3840, %rcx                     # imm = 0xF00
.Ltmp74:
	.loc	4 38 5 is_stmt 0                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:38:5
	jne	.LBB6_16
.Ltmp75:
# %bb.17:
	#DEBUG_VALUE: vga_putchar:c <- [DW_OP_LLVM_entry_value 1] $edi
	#DEBUG_VALUE: x <- 0
	.loc	4 0 5                           # C:\Users\Gower\Downloads\BojanOS\src\vga.c:0:5
	movzbl	color(%rip), %eax
	shll	$8, %eax
	orl	$32, %eax
.Ltmp76:
	.loc	4 41 5 is_stmt 1                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:41:5
	movd	%eax, %xmm0
	pshuflw	$0, %xmm0, %xmm0                # xmm0 = xmm0[0,0,0,0,4,5,6,7]
	pshufd	$0, %xmm0, %xmm0                # xmm0 = xmm0[0,0,0,0]
.Ltmp77:
	.loc	4 42 54                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:42:54
	movdqa	%xmm0, 757504
	movdqa	%xmm0, 757520
	movdqa	%xmm0, 757536
	movdqa	%xmm0, 757552
	movdqa	%xmm0, 757568
	movdqa	%xmm0, 757584
	movdqa	%xmm0, 757600
	movdqa	%xmm0, 757616
	movdqa	%xmm0, 757632
	movdqa	%xmm0, 757648
.Ltmp78:
	.loc	4 43 9                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:43:9
	movq	$24, row(%rip)
	.loc	4 44 9                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:44:9
	movq	$0, col(%rip)
	movl	$24, %ecx
	xorl	%edx, %edx
.Ltmp79:
.LBB6_18:
	#DEBUG_VALUE: vga_putchar:c <- [DW_OP_LLVM_entry_value 1] $edi
	#DEBUG_VALUE: vga_setcursor:x <- $rdx
	#DEBUG_VALUE: vga_setcursor:y <- $rcx
	.loc	4 23 22                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:23:22
	leaq	(%rcx,%rcx,4), %rcx
.Ltmp80:
	shlq	$4, %rcx
	.loc	4 23 34 is_stmt 0               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:23:34
	addq	%rdx, %rcx
.Ltmp81:
	#DEBUG_VALUE: vga_setcursor:pos <- [DW_OP_LLVM_convert 64 7, DW_OP_LLVM_convert 16 7, DW_OP_stack_value] $rcx
	#DEBUG_VALUE: outb:port <- 980
	#DEBUG_VALUE: outb:val <- 14
	.loc	5 5 5 is_stmt 1                 # C:\Users\Gower\Downloads\BojanOS\include/io.h:5:5
	movb	$14, %al
	movw	$980, %dx                       # imm = 0x3D4
.Ltmp82:
	#APP
	outb	%al, %dx
	#NO_APP
.Ltmp83:
	.loc	4 25 17                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:25:17
	movl	%ecx, %eax
	shrl	$8, %eax
.Ltmp84:
	#DEBUG_VALUE: outb:val <- $al
	#DEBUG_VALUE: outb:port <- 981
                                        # kill: def $al killed $al killed $eax
	.loc	5 5 5                           # C:\Users\Gower\Downloads\BojanOS\include/io.h:5:5
	movw	$981, %dx                       # imm = 0x3D5
	#APP
	outb	%al, %dx
	#NO_APP
.Ltmp85:
	#DEBUG_VALUE: outb:port <- 980
	#DEBUG_VALUE: outb:val <- 15
	.loc	5 5 5 is_stmt 0                 # C:\Users\Gower\Downloads\BojanOS\include/io.h:5:5
	movb	$15, %al
.Ltmp86:
	movw	$980, %dx                       # imm = 0x3D4
	#APP
	outb	%al, %dx
	#NO_APP
.Ltmp87:
	#DEBUG_VALUE: outb:port <- 981
	#DEBUG_VALUE: outb:val <- $cl
	.loc	5 5 5                           # C:\Users\Gower\Downloads\BojanOS\include/io.h:5:5
	movl	%ecx, %eax
	movw	$981, %dx                       # imm = 0x3D5
	#APP
	outb	%al, %dx
	#NO_APP
.Ltmp88:
	.loc	4 74 1 is_stmt 1                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:74:1
	retq
.Ltmp89:
.Lfunc_end6:
	.size	vga_putchar, .Lfunc_end6-vga_putchar
	.cfi_endproc
                                        # -- End function
	.globl	vga_write                       # -- Begin function vga_write
	.p2align	4, 0x90
	.type	vga_write,@function
vga_write:                              # @vga_write
.Lfunc_begin7:
	.cfi_startproc
# %bb.0:
	#DEBUG_VALUE: vga_write:data <- $rdi
	#DEBUG_VALUE: vga_write:size <- $rsi
	#DEBUG_VALUE: i <- 0
	.loc	4 76 26 prologue_end            # C:\Users\Gower\Downloads\BojanOS\src\vga.c:76:26
	testq	%rsi, %rsi
.Ltmp90:
	.loc	4 76 5 is_stmt 0                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:76:5
	je	.LBB7_4
.Ltmp91:
# %bb.1:
	#DEBUG_VALUE: vga_write:data <- $rdi
	#DEBUG_VALUE: vga_write:size <- $rsi
	#DEBUG_VALUE: i <- 0
	pushq	%r15
	.cfi_def_cfa_offset 16
	pushq	%r14
	.cfi_def_cfa_offset 24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset %rbx, -32
	.cfi_offset %r14, -24
	.cfi_offset %r15, -16
	movq	%rsi, %rbx
	movq	%rdi, %r14
	xorl	%r15d, %r15d
.Ltmp92:
	.p2align	4, 0x90
.LBB7_2:                                # =>This Inner Loop Header: Depth=1
	#DEBUG_VALUE: vga_write:data <- $r14
	#DEBUG_VALUE: vga_write:size <- $rbx
	#DEBUG_VALUE: i <- $r15
	.loc	4 77 21 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:77:21
	movsbl	(%r14,%r15), %edi
	.loc	4 77 9 is_stmt 0                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:77:9
	callq	vga_putchar
.Ltmp93:
	.loc	4 76 35 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:76:35
	addq	$1, %r15
.Ltmp94:
	#DEBUG_VALUE: i <- $r15
	.loc	4 76 26 is_stmt 0               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:76:26
	cmpq	%r15, %rbx
.Ltmp95:
	.loc	4 76 5                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:76:5
	jne	.LBB7_2
.Ltmp96:
# %bb.3:
	#DEBUG_VALUE: vga_write:data <- $r14
	#DEBUG_VALUE: vga_write:size <- $rbx
	#DEBUG_VALUE: i <- $r15
	.loc	4 0 5                           # C:\Users\Gower\Downloads\BojanOS\src\vga.c:0:5
	popq	%rbx
.Ltmp97:
	#DEBUG_VALUE: vga_write:size <- [DW_OP_LLVM_entry_value 1] $rsi
	.cfi_def_cfa_offset 24
	popq	%r14
.Ltmp98:
	#DEBUG_VALUE: vga_write:data <- [DW_OP_LLVM_entry_value 1] $rdi
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	.cfi_restore %rbx
	.cfi_restore %r14
	.cfi_restore %r15
.Ltmp99:
.LBB7_4:
	#DEBUG_VALUE: vga_write:data <- [DW_OP_LLVM_entry_value 1] $rdi
	#DEBUG_VALUE: vga_write:size <- [DW_OP_LLVM_entry_value 1] $rsi
	.loc	4 78 1 is_stmt 1                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:78:1
	retq
.Ltmp100:
.Lfunc_end7:
	.size	vga_write, .Lfunc_end7-vga_write
	.cfi_endproc
                                        # -- End function
	.globl	vga_print                       # -- Begin function vga_print
	.p2align	4, 0x90
	.type	vga_print,@function
vga_print:                              # @vga_print
.Lfunc_begin8:
	.loc	4 79 0                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:79:0
	.cfi_startproc
# %bb.0:
	#DEBUG_VALUE: vga_print:data <- $rdi
	pushq	%r15
	.cfi_def_cfa_offset 16
	pushq	%r14
	.cfi_def_cfa_offset 24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset %rbx, -32
	.cfi_offset %r14, -24
	.cfi_offset %r15, -16
	movq	%rdi, %rbx
	movq	$-1, %r14
.Ltmp101:
	#DEBUG_VALUE: vga_print:len <- 0
	.p2align	4, 0x90
.LBB8_1:                                # =>This Inner Loop Header: Depth=1
	#DEBUG_VALUE: vga_print:data <- $rbx
	#DEBUG_VALUE: vga_print:len <- [DW_OP_plus_uconst 1, DW_OP_stack_value] $r14
	#DEBUG_VALUE: vga_print:len <- [DW_OP_plus_uconst 2, DW_OP_stack_value] $r14
	.loc	4 81 5 prologue_end             # C:\Users\Gower\Downloads\BojanOS\src\vga.c:81:5
	cmpb	$0, 1(%rbx,%r14)
	leaq	1(%r14), %r14
.Ltmp102:
	jne	.LBB8_1
.Ltmp103:
# %bb.2:
	#DEBUG_VALUE: vga_print:data <- $rbx
	#DEBUG_VALUE: vga_write:data <- $rbx
	#DEBUG_VALUE: vga_write:size <- undef
	#DEBUG_VALUE: i <- 0
	.loc	4 76 26                         # C:\Users\Gower\Downloads\BojanOS\src\vga.c:76:26
	testq	%r14, %r14
.Ltmp104:
	.loc	4 76 5 is_stmt 0                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:76:5
	je	.LBB8_5
.Ltmp105:
# %bb.3:
	#DEBUG_VALUE: vga_print:data <- $rbx
	#DEBUG_VALUE: vga_write:data <- $rbx
	#DEBUG_VALUE: i <- 0
	.loc	4 0 5                           # C:\Users\Gower\Downloads\BojanOS\src\vga.c:0:5
	xorl	%r15d, %r15d
.Ltmp106:
	.p2align	4, 0x90
.LBB8_4:                                # =>This Inner Loop Header: Depth=1
	#DEBUG_VALUE: vga_print:data <- $rbx
	#DEBUG_VALUE: vga_write:data <- $rbx
	#DEBUG_VALUE: i <- $r15
	.loc	4 77 21 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:77:21
	movsbl	(%rbx,%r15), %edi
	.loc	4 77 9 is_stmt 0                # C:\Users\Gower\Downloads\BojanOS\src\vga.c:77:9
	callq	vga_putchar
.Ltmp107:
	.loc	4 76 35 is_stmt 1               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:76:35
	addq	$1, %r15
.Ltmp108:
	#DEBUG_VALUE: i <- $r15
	.loc	4 76 26 is_stmt 0               # C:\Users\Gower\Downloads\BojanOS\src\vga.c:76:26
	cmpq	%r15, %r14
.Ltmp109:
	.loc	4 76 5                          # C:\Users\Gower\Downloads\BojanOS\src\vga.c:76:5
	jne	.LBB8_4
.Ltmp110:
.LBB8_5:
	#DEBUG_VALUE: vga_print:data <- $rbx
	.loc	4 83 1 epilogue_begin is_stmt 1 # C:\Users\Gower\Downloads\BojanOS\src\vga.c:83:1
	popq	%rbx
.Ltmp111:
	#DEBUG_VALUE: vga_print:data <- [DW_OP_LLVM_entry_value 1] $rdi
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	retq
.Ltmp112:
.Lfunc_end8:
	.size	vga_print, .Lfunc_end8-vga_print
	.cfi_endproc
                                        # -- End function
	.type	row,@object                     # @row
	.local	row
	.comm	row,8,8
	.type	col,@object                     # @col
	.local	col
	.comm	col,8,8
	.type	color,@object                   # @color
	.local	color
	.comm	color,1,1
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
	.quad	.Ltmp1-.Lfunc_begin0
	.quad	.Ltmp2-.Lfunc_begin0
	.short	7                               # Loc expr size
	.byte	114                             # DW_OP_breg2
	.byte	0                               # 0
	.byte	17                              # DW_OP_consts
	.byte	160                             # 160
	.byte	1                               # 
	.byte	27                              # DW_OP_div
	.byte	159                             # DW_OP_stack_value
	.quad	.Ltmp2-.Lfunc_begin0
	.quad	.Ltmp3-.Lfunc_begin0
	.short	10                              # Loc expr size
	.byte	114                             # DW_OP_breg2
	.byte	0                               # 0
	.byte	17                              # DW_OP_consts
	.byte	160                             # 160
	.byte	1                               # 
	.byte	27                              # DW_OP_div
	.byte	17                              # DW_OP_consts
	.byte	1                               # 1
	.byte	34                              # DW_OP_plus
	.byte	159                             # DW_OP_stack_value
	.quad	0
	.quad	0
.Ldebug_loc1:
	.quad	.Lfunc_begin1-.Lfunc_begin0
	.quad	.Ltmp9-.Lfunc_begin0
	.short	2                               # Loc expr size
	.byte	48                              # DW_OP_lit0
	.byte	159                             # DW_OP_stack_value
	.quad	.Ltmp9-.Lfunc_begin0
	.quad	.Ltmp10-.Lfunc_begin0
	.short	7                               # Loc expr size
	.byte	114                             # DW_OP_breg2
	.byte	0                               # 0
	.byte	17                              # DW_OP_consts
	.byte	160                             # 160
	.byte	1                               # 
	.byte	27                              # DW_OP_div
	.byte	159                             # DW_OP_stack_value
	.quad	.Ltmp10-.Lfunc_begin0
	.quad	.Ltmp11-.Lfunc_begin0
	.short	10                              # Loc expr size
	.byte	114                             # DW_OP_breg2
	.byte	0                               # 0
	.byte	17                              # DW_OP_consts
	.byte	160                             # 160
	.byte	1                               # 
	.byte	27                              # DW_OP_div
	.byte	17                              # DW_OP_consts
	.byte	1                               # 1
	.byte	34                              # DW_OP_plus
	.byte	159                             # DW_OP_stack_value
	.quad	0
	.quad	0
.Ldebug_loc2:
	.quad	.Ltmp9-.Lfunc_begin0
	.quad	.Ltmp13-.Lfunc_begin0
	.short	2                               # Loc expr size
	.byte	48                              # DW_OP_lit0
	.byte	159                             # DW_OP_stack_value
	.quad	0
	.quad	0
.Ldebug_loc3:
	.quad	.Ltmp16-.Lfunc_begin0
	.quad	.Lfunc_end3-.Lfunc_begin0
	.short	3                               # Loc expr size
	.byte	114                             # DW_OP_breg2
	.byte	0                               # 0
	.byte	159                             # DW_OP_stack_value
	.quad	0
	.quad	0
.Ldebug_loc4:
	.quad	.Ltmp24-.Lfunc_begin0
	.quad	.Ltmp25-.Lfunc_begin0
	.short	7                               # Loc expr size
	.byte	114                             # DW_OP_breg2
	.byte	0                               # 0
	.byte	17                              # DW_OP_consts
	.byte	160                             # 160
	.byte	1                               # 
	.byte	27                              # DW_OP_div
	.byte	159                             # DW_OP_stack_value
	.quad	.Ltmp25-.Lfunc_begin0
	.quad	.Ltmp26-.Lfunc_begin0
	.short	10                              # Loc expr size
	.byte	114                             # DW_OP_breg2
	.byte	0                               # 0
	.byte	17                              # DW_OP_consts
	.byte	160                             # 160
	.byte	1                               # 
	.byte	27                              # DW_OP_div
	.byte	17                              # DW_OP_consts
	.byte	1                               # 1
	.byte	34                              # DW_OP_plus
	.byte	159                             # DW_OP_stack_value
	.quad	0
	.quad	0
.Ldebug_loc5:
	.quad	.Ltmp38-.Lfunc_begin0
	.quad	.Ltmp44-.Lfunc_begin0
	.short	3                               # Loc expr size
	.byte	114                             # DW_OP_breg2
	.byte	0                               # 0
	.byte	159                             # DW_OP_stack_value
	.quad	0
	.quad	0
.Ldebug_loc6:
	.quad	.Lfunc_begin6-.Lfunc_begin0
	.quad	.Ltmp52-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	85                              # super-register DW_OP_reg5
	.quad	.Ltmp52-.Lfunc_begin0
	.quad	.Lfunc_end6-.Lfunc_begin0
	.short	4                               # Loc expr size
	.byte	243                             # DW_OP_GNU_entry_value
	.byte	1                               # 1
	.byte	85                              # super-register DW_OP_reg5
	.byte	159                             # DW_OP_stack_value
	.quad	0
	.quad	0
.Ldebug_loc7:
	.quad	.Ltmp51-.Lfunc_begin0
	.quad	.Ltmp54-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	84                              # DW_OP_reg4
	.quad	0
	.quad	0
.Ldebug_loc8:
	.quad	.Ltmp71-.Lfunc_begin0
	.quad	.Ltmp72-.Lfunc_begin0
	.short	7                               # Loc expr size
	.byte	114                             # DW_OP_breg2
	.byte	0                               # 0
	.byte	17                              # DW_OP_consts
	.byte	160                             # 160
	.byte	1                               # 
	.byte	27                              # DW_OP_div
	.byte	159                             # DW_OP_stack_value
	.quad	.Ltmp72-.Lfunc_begin0
	.quad	.Ltmp73-.Lfunc_begin0
	.short	10                              # Loc expr size
	.byte	114                             # DW_OP_breg2
	.byte	0                               # 0
	.byte	17                              # DW_OP_consts
	.byte	160                             # 160
	.byte	1                               # 
	.byte	27                              # DW_OP_div
	.byte	17                              # DW_OP_consts
	.byte	1                               # 1
	.byte	34                              # DW_OP_plus
	.byte	159                             # DW_OP_stack_value
	.quad	0
	.quad	0
.Ldebug_loc9:
	.quad	.Ltmp79-.Lfunc_begin0
	.quad	.Ltmp82-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	81                              # DW_OP_reg1
	.quad	0
	.quad	0
.Ldebug_loc10:
	.quad	.Ltmp79-.Lfunc_begin0
	.quad	.Ltmp80-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	82                              # DW_OP_reg2
	.quad	0
	.quad	0
.Ldebug_loc11:
	.quad	.Ltmp81-.Lfunc_begin0
	.quad	.Lfunc_end6-.Lfunc_begin0
	.short	3                               # Loc expr size
	.byte	114                             # DW_OP_breg2
	.byte	0                               # 0
	.byte	159                             # DW_OP_stack_value
	.quad	0
	.quad	0
.Ldebug_loc12:
	.quad	.Lfunc_begin7-.Lfunc_begin0
	.quad	.Ltmp92-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	85                              # DW_OP_reg5
	.quad	.Ltmp92-.Lfunc_begin0
	.quad	.Ltmp98-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	94                              # DW_OP_reg14
	.quad	.Ltmp98-.Lfunc_begin0
	.quad	.Lfunc_end7-.Lfunc_begin0
	.short	4                               # Loc expr size
	.byte	243                             # DW_OP_GNU_entry_value
	.byte	1                               # 1
	.byte	85                              # DW_OP_reg5
	.byte	159                             # DW_OP_stack_value
	.quad	0
	.quad	0
.Ldebug_loc13:
	.quad	.Lfunc_begin7-.Lfunc_begin0
	.quad	.Ltmp92-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	84                              # DW_OP_reg4
	.quad	.Ltmp92-.Lfunc_begin0
	.quad	.Ltmp97-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	83                              # DW_OP_reg3
	.quad	.Ltmp97-.Lfunc_begin0
	.quad	.Lfunc_end7-.Lfunc_begin0
	.short	4                               # Loc expr size
	.byte	243                             # DW_OP_GNU_entry_value
	.byte	1                               # 1
	.byte	84                              # DW_OP_reg4
	.byte	159                             # DW_OP_stack_value
	.quad	0
	.quad	0
.Ldebug_loc14:
	.quad	.Lfunc_begin7-.Lfunc_begin0
	.quad	.Ltmp92-.Lfunc_begin0
	.short	2                               # Loc expr size
	.byte	48                              # DW_OP_lit0
	.byte	159                             # DW_OP_stack_value
	.quad	.Ltmp92-.Lfunc_begin0
	.quad	.Ltmp96-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	95                              # DW_OP_reg15
	.quad	0
	.quad	0
.Ldebug_loc15:
	.quad	.Lfunc_begin8-.Lfunc_begin0
	.quad	.Ltmp101-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	85                              # DW_OP_reg5
	.quad	.Ltmp101-.Lfunc_begin0
	.quad	.Ltmp111-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	83                              # DW_OP_reg3
	.quad	.Ltmp111-.Lfunc_begin0
	.quad	.Lfunc_end8-.Lfunc_begin0
	.short	4                               # Loc expr size
	.byte	243                             # DW_OP_GNU_entry_value
	.byte	1                               # 1
	.byte	85                              # DW_OP_reg5
	.byte	159                             # DW_OP_stack_value
	.quad	0
	.quad	0
.Ldebug_loc16:
	.quad	.Ltmp103-.Lfunc_begin0
	.quad	.Ltmp106-.Lfunc_begin0
	.short	2                               # Loc expr size
	.byte	48                              # DW_OP_lit0
	.byte	159                             # DW_OP_stack_value
	.quad	.Ltmp106-.Lfunc_begin0
	.quad	.Ltmp110-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	95                              # DW_OP_reg15
	.quad	0
	.quad	0
	.section	.debug_abbrev,"",@progbits
	.byte	1                               # Abbreviation Code
	.byte	17                              # DW_TAG_compile_unit
	.byte	1                               # DW_CHILDREN_yes
	.byte	37                              # DW_AT_producer
	.byte	14                              # DW_FORM_strp
	.byte	19                              # DW_AT_language
	.byte	5                               # DW_FORM_data2
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	16                              # DW_AT_stmt_list
	.byte	23                              # DW_FORM_sec_offset
	.byte	27                              # DW_AT_comp_dir
	.byte	14                              # DW_FORM_strp
	.byte	17                              # DW_AT_low_pc
	.byte	1                               # DW_FORM_addr
	.byte	18                              # DW_AT_high_pc
	.byte	6                               # DW_FORM_data4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	2                               # Abbreviation Code
	.byte	52                              # DW_TAG_variable
	.byte	0                               # DW_CHILDREN_no
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	28                              # DW_AT_const_value
	.byte	15                              # DW_FORM_udata
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	3                               # Abbreviation Code
	.byte	38                              # DW_TAG_const_type
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	4                               # Abbreviation Code
	.byte	22                              # DW_TAG_typedef
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	5                               # Abbreviation Code
	.byte	36                              # DW_TAG_base_type
	.byte	0                               # DW_CHILDREN_no
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	62                              # DW_AT_encoding
	.byte	11                              # DW_FORM_data1
	.byte	11                              # DW_AT_byte_size
	.byte	11                              # DW_FORM_data1
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	6                               # Abbreviation Code
	.byte	52                              # DW_TAG_variable
	.byte	0                               # DW_CHILDREN_no
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	7                               # Abbreviation Code
	.byte	15                              # DW_TAG_pointer_type
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	8                               # Abbreviation Code
	.byte	52                              # DW_TAG_variable
	.byte	0                               # DW_CHILDREN_no
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	2                               # DW_AT_location
	.byte	24                              # DW_FORM_exprloc
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	9                               # Abbreviation Code
	.byte	22                              # DW_TAG_typedef
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	5                               # DW_FORM_data2
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	10                              # Abbreviation Code
	.byte	46                              # DW_TAG_subprogram
	.byte	1                               # DW_CHILDREN_yes
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	39                              # DW_AT_prototyped
	.byte	25                              # DW_FORM_flag_present
	.byte	63                              # DW_AT_external
	.byte	25                              # DW_FORM_flag_present
	.byte	32                              # DW_AT_inline
	.byte	11                              # DW_FORM_data1
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	11                              # Abbreviation Code
	.byte	11                              # DW_TAG_lexical_block
	.byte	1                               # DW_CHILDREN_yes
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	12                              # Abbreviation Code
	.byte	52                              # DW_TAG_variable
	.byte	0                               # DW_CHILDREN_no
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	13                              # Abbreviation Code
	.byte	46                              # DW_TAG_subprogram
	.byte	1                               # DW_CHILDREN_yes
	.byte	17                              # DW_AT_low_pc
	.byte	1                               # DW_FORM_addr
	.byte	18                              # DW_AT_high_pc
	.byte	6                               # DW_FORM_data4
	.byte	64                              # DW_AT_frame_base
	.byte	24                              # DW_FORM_exprloc
	.ascii	"\227B"                         # DW_AT_GNU_all_call_sites
	.byte	25                              # DW_FORM_flag_present
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	39                              # DW_AT_prototyped
	.byte	25                              # DW_FORM_flag_present
	.byte	63                              # DW_AT_external
	.byte	25                              # DW_FORM_flag_present
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	14                              # Abbreviation Code
	.byte	29                              # DW_TAG_inlined_subroutine
	.byte	1                               # DW_CHILDREN_yes
	.byte	49                              # DW_AT_abstract_origin
	.byte	19                              # DW_FORM_ref4
	.byte	17                              # DW_AT_low_pc
	.byte	1                               # DW_FORM_addr
	.byte	18                              # DW_AT_high_pc
	.byte	6                               # DW_FORM_data4
	.byte	88                              # DW_AT_call_file
	.byte	11                              # DW_FORM_data1
	.byte	89                              # DW_AT_call_line
	.byte	11                              # DW_FORM_data1
	.byte	87                              # DW_AT_call_column
	.byte	11                              # DW_FORM_data1
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	15                              # Abbreviation Code
	.byte	11                              # DW_TAG_lexical_block
	.byte	1                               # DW_CHILDREN_yes
	.byte	17                              # DW_AT_low_pc
	.byte	1                               # DW_FORM_addr
	.byte	18                              # DW_AT_high_pc
	.byte	6                               # DW_FORM_data4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	16                              # Abbreviation Code
	.byte	52                              # DW_TAG_variable
	.byte	0                               # DW_CHILDREN_no
	.byte	2                               # DW_AT_location
	.byte	23                              # DW_FORM_sec_offset
	.byte	49                              # DW_AT_abstract_origin
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	17                              # Abbreviation Code
	.byte	52                              # DW_TAG_variable
	.byte	0                               # DW_CHILDREN_no
	.byte	28                              # DW_AT_const_value
	.byte	15                              # DW_FORM_udata
	.byte	49                              # DW_AT_abstract_origin
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	18                              # Abbreviation Code
	.byte	46                              # DW_TAG_subprogram
	.byte	1                               # DW_CHILDREN_yes
	.byte	17                              # DW_AT_low_pc
	.byte	1                               # DW_FORM_addr
	.byte	18                              # DW_AT_high_pc
	.byte	6                               # DW_FORM_data4
	.byte	64                              # DW_AT_frame_base
	.byte	24                              # DW_FORM_exprloc
	.ascii	"\227B"                         # DW_AT_GNU_all_call_sites
	.byte	25                              # DW_FORM_flag_present
	.byte	49                              # DW_AT_abstract_origin
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	19                              # Abbreviation Code
	.byte	5                               # DW_TAG_formal_parameter
	.byte	0                               # DW_CHILDREN_no
	.byte	2                               # DW_AT_location
	.byte	24                              # DW_FORM_exprloc
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	20                              # Abbreviation Code
	.byte	46                              # DW_TAG_subprogram
	.byte	1                               # DW_CHILDREN_yes
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	39                              # DW_AT_prototyped
	.byte	25                              # DW_FORM_flag_present
	.byte	32                              # DW_AT_inline
	.byte	11                              # DW_FORM_data1
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	21                              # Abbreviation Code
	.byte	5                               # DW_TAG_formal_parameter
	.byte	0                               # DW_CHILDREN_no
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	22                              # Abbreviation Code
	.byte	5                               # DW_TAG_formal_parameter
	.byte	0                               # DW_CHILDREN_no
	.byte	2                               # DW_AT_location
	.byte	24                              # DW_FORM_exprloc
	.byte	49                              # DW_AT_abstract_origin
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	23                              # Abbreviation Code
	.byte	5                               # DW_TAG_formal_parameter
	.byte	0                               # DW_CHILDREN_no
	.byte	28                              # DW_AT_const_value
	.byte	15                              # DW_FORM_udata
	.byte	49                              # DW_AT_abstract_origin
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	24                              # Abbreviation Code
	.byte	46                              # DW_TAG_subprogram
	.byte	1                               # DW_CHILDREN_yes
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	39                              # DW_AT_prototyped
	.byte	25                              # DW_FORM_flag_present
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	32                              # DW_AT_inline
	.byte	11                              # DW_FORM_data1
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	25                              # Abbreviation Code
	.byte	5                               # DW_TAG_formal_parameter
	.byte	0                               # DW_CHILDREN_no
	.byte	2                               # DW_AT_location
	.byte	23                              # DW_FORM_sec_offset
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	26                              # Abbreviation Code
	.byte	52                              # DW_TAG_variable
	.byte	0                               # DW_CHILDREN_no
	.byte	2                               # DW_AT_location
	.byte	23                              # DW_FORM_sec_offset
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	27                              # Abbreviation Code
	.byte	5                               # DW_TAG_formal_parameter
	.byte	0                               # DW_CHILDREN_no
	.byte	2                               # DW_AT_location
	.byte	23                              # DW_FORM_sec_offset
	.byte	49                              # DW_AT_abstract_origin
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	28                              # Abbreviation Code
	.ascii	"\211\202\001"                  # DW_TAG_GNU_call_site
	.byte	0                               # DW_CHILDREN_no
	.byte	49                              # DW_AT_abstract_origin
	.byte	19                              # DW_FORM_ref4
	.byte	17                              # DW_AT_low_pc
	.byte	1                               # DW_FORM_addr
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	29                              # Abbreviation Code
	.byte	52                              # DW_TAG_variable
	.byte	0                               # DW_CHILDREN_no
	.byte	2                               # DW_AT_location
	.byte	24                              # DW_FORM_exprloc
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	0                               # EOM(3)
	.section	.debug_info,"",@progbits
.Lcu_begin0:
	.long	.Ldebug_info_end0-.Ldebug_info_start0 # Length of Unit
.Ldebug_info_start0:
	.short	4                               # DWARF version number
	.long	.debug_abbrev                   # Offset Into Abbrev. Section
	.byte	8                               # Address Size (in bytes)
	.byte	1                               # Abbrev [1] 0xb:0x68a DW_TAG_compile_unit
	.long	.Linfo_string0                  # DW_AT_producer
	.short	29                              # DW_AT_language
	.long	.Linfo_string1                  # DW_AT_name
	.long	.Lline_table_start0             # DW_AT_stmt_list
	.long	.Linfo_string2                  # DW_AT_comp_dir
	.quad	.Lfunc_begin0                   # DW_AT_low_pc
	.long	.Lfunc_end8-.Lfunc_begin0       # DW_AT_high_pc
	.byte	2                               # Abbrev [2] 0x2a:0xc DW_TAG_variable
	.long	.Linfo_string3                  # DW_AT_name
	.long	54                              # DW_AT_type
	.byte	2                               # DW_AT_decl_file
	.byte	5                               # DW_AT_decl_line
	.byte	80                              # DW_AT_const_value
	.byte	3                               # Abbrev [3] 0x36:0x5 DW_TAG_const_type
	.long	59                              # DW_AT_type
	.byte	4                               # Abbrev [4] 0x3b:0xb DW_TAG_typedef
	.long	70                              # DW_AT_type
	.long	.Linfo_string5                  # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	18                              # DW_AT_decl_line
	.byte	5                               # Abbrev [5] 0x46:0x7 DW_TAG_base_type
	.long	.Linfo_string4                  # DW_AT_name
	.byte	7                               # DW_AT_encoding
	.byte	8                               # DW_AT_byte_size
	.byte	2                               # Abbrev [2] 0x4d:0xc DW_TAG_variable
	.long	.Linfo_string6                  # DW_AT_name
	.long	54                              # DW_AT_type
	.byte	2                               # DW_AT_decl_file
	.byte	6                               # DW_AT_decl_line
	.byte	25                              # DW_AT_const_value
	.byte	6                               # Abbrev [6] 0x59:0xb DW_TAG_variable
	.long	.Linfo_string7                  # DW_AT_name
	.long	100                             # DW_AT_type
	.byte	4                               # DW_AT_decl_file
	.byte	3                               # DW_AT_decl_line
	.byte	3                               # Abbrev [3] 0x64:0x5 DW_TAG_const_type
	.long	105                             # DW_AT_type
	.byte	7                               # Abbrev [7] 0x69:0x5 DW_TAG_pointer_type
	.long	110                             # DW_AT_type
	.byte	4                               # Abbrev [4] 0x6e:0xb DW_TAG_typedef
	.long	121                             # DW_AT_type
	.long	.Linfo_string9                  # DW_AT_name
	.byte	3                               # DW_AT_decl_file
	.byte	243                             # DW_AT_decl_line
	.byte	5                               # Abbrev [5] 0x79:0x7 DW_TAG_base_type
	.long	.Linfo_string8                  # DW_AT_name
	.byte	7                               # DW_AT_encoding
	.byte	2                               # DW_AT_byte_size
	.byte	8                               # Abbrev [8] 0x80:0x15 DW_TAG_variable
	.long	.Linfo_string10                 # DW_AT_name
	.long	59                              # DW_AT_type
	.byte	4                               # DW_AT_decl_file
	.byte	4                               # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	row
	.byte	8                               # Abbrev [8] 0x95:0x15 DW_TAG_variable
	.long	.Linfo_string11                 # DW_AT_name
	.long	59                              # DW_AT_type
	.byte	4                               # DW_AT_decl_file
	.byte	5                               # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	col
	.byte	8                               # Abbrev [8] 0xaa:0x15 DW_TAG_variable
	.long	.Linfo_string12                 # DW_AT_name
	.long	191                             # DW_AT_type
	.byte	4                               # DW_AT_decl_file
	.byte	6                               # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	color
	.byte	9                               # Abbrev [9] 0xbf:0xc DW_TAG_typedef
	.long	203                             # DW_AT_type
	.long	.Linfo_string14                 # DW_AT_name
	.byte	3                               # DW_AT_decl_file
	.short	266                             # DW_AT_decl_line
	.byte	5                               # Abbrev [5] 0xcb:0x7 DW_TAG_base_type
	.long	.Linfo_string13                 # DW_AT_name
	.byte	8                               # DW_AT_encoding
	.byte	1                               # DW_AT_byte_size
	.byte	10                              # Abbrev [10] 0xd2:0x23 DW_TAG_subprogram
	.long	.Linfo_string15                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	29                              # DW_AT_decl_line
                                        # DW_AT_prototyped
                                        # DW_AT_external
	.byte	1                               # DW_AT_inline
	.byte	11                              # Abbrev [11] 0xda:0x1a DW_TAG_lexical_block
	.byte	12                              # Abbrev [12] 0xdb:0xb DW_TAG_variable
	.long	.Linfo_string16                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	30                              # DW_AT_decl_line
	.long	59                              # DW_AT_type
	.byte	11                              # Abbrev [11] 0xe6:0xd DW_TAG_lexical_block
	.byte	12                              # Abbrev [12] 0xe7:0xb DW_TAG_variable
	.long	.Linfo_string17                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	31                              # DW_AT_decl_line
	.long	59                              # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	13                              # Abbrev [13] 0xf5:0x56 DW_TAG_subprogram
	.quad	.Lfunc_begin0                   # DW_AT_low_pc
	.long	.Lfunc_end0-.Lfunc_begin0       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
                                        # DW_AT_GNU_all_call_sites
	.long	.Linfo_string31                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	13                              # DW_AT_decl_line
                                        # DW_AT_prototyped
                                        # DW_AT_external
	.byte	14                              # Abbrev [14] 0x10a:0x40 DW_TAG_inlined_subroutine
	.long	210                             # DW_AT_abstract_origin
	.quad	.Ltmp1                          # DW_AT_low_pc
	.long	.Ltmp6-.Ltmp1                   # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	17                              # DW_AT_call_line
	.byte	5                               # DW_AT_call_column
	.byte	15                              # Abbrev [15] 0x11e:0x2b DW_TAG_lexical_block
	.quad	.Ltmp1                          # DW_AT_low_pc
	.long	.Ltmp5-.Ltmp1                   # DW_AT_high_pc
	.byte	16                              # Abbrev [16] 0x12b:0x9 DW_TAG_variable
	.long	.Ldebug_loc0                    # DW_AT_location
	.long	219                             # DW_AT_abstract_origin
	.byte	15                              # Abbrev [15] 0x134:0x14 DW_TAG_lexical_block
	.quad	.Ltmp1                          # DW_AT_low_pc
	.long	.Ltmp2-.Ltmp1                   # DW_AT_high_pc
	.byte	17                              # Abbrev [17] 0x141:0x6 DW_TAG_variable
	.byte	0                               # DW_AT_const_value
	.long	231                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	18                              # Abbrev [18] 0x14b:0x42 DW_TAG_subprogram
	.quad	.Lfunc_begin1                   # DW_AT_low_pc
	.long	.Lfunc_end1-.Lfunc_begin1       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
                                        # DW_AT_GNU_all_call_sites
	.long	210                             # DW_AT_abstract_origin
	.byte	15                              # Abbrev [15] 0x15e:0x2e DW_TAG_lexical_block
	.quad	.Ltmp8                          # DW_AT_low_pc
	.long	.Ltmp13-.Ltmp8                  # DW_AT_high_pc
	.byte	16                              # Abbrev [16] 0x16b:0x9 DW_TAG_variable
	.long	.Ldebug_loc1                    # DW_AT_location
	.long	219                             # DW_AT_abstract_origin
	.byte	15                              # Abbrev [15] 0x174:0x17 DW_TAG_lexical_block
	.quad	.Ltmp8                          # DW_AT_low_pc
	.long	.Ltmp10-.Ltmp8                  # DW_AT_high_pc
	.byte	16                              # Abbrev [16] 0x181:0x9 DW_TAG_variable
	.long	.Ldebug_loc2                    # DW_AT_location
	.long	231                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	13                              # Abbrev [13] 0x18d:0x23 DW_TAG_subprogram
	.quad	.Lfunc_begin2                   # DW_AT_low_pc
	.long	.Lfunc_end2-.Lfunc_begin2       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
                                        # DW_AT_GNU_all_call_sites
	.long	.Linfo_string32                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	19                              # DW_AT_decl_line
                                        # DW_AT_prototyped
                                        # DW_AT_external
	.byte	19                              # Abbrev [19] 0x1a2:0xd DW_TAG_formal_parameter
	.byte	1                               # DW_AT_location
	.byte	85
	.long	.Linfo_string22                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	19                              # DW_AT_decl_line
	.long	191                             # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	20                              # Abbrev [20] 0x1b0:0x1f DW_TAG_subprogram
	.long	.Linfo_string18                 # DW_AT_name
	.byte	5                               # DW_AT_decl_file
	.byte	4                               # DW_AT_decl_line
                                        # DW_AT_prototyped
	.byte	1                               # DW_AT_inline
	.byte	21                              # Abbrev [21] 0x1b8:0xb DW_TAG_formal_parameter
	.long	.Linfo_string19                 # DW_AT_name
	.byte	5                               # DW_AT_decl_file
	.byte	4                               # DW_AT_decl_line
	.long	110                             # DW_AT_type
	.byte	21                              # Abbrev [21] 0x1c3:0xb DW_TAG_formal_parameter
	.long	.Linfo_string20                 # DW_AT_name
	.byte	5                               # DW_AT_decl_file
	.byte	4                               # DW_AT_decl_line
	.long	191                             # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	18                              # Abbrev [18] 0x1cf:0xb5 DW_TAG_subprogram
	.quad	.Lfunc_begin3                   # DW_AT_low_pc
	.long	.Lfunc_end3-.Lfunc_begin3       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
                                        # DW_AT_GNU_all_call_sites
	.long	762                             # DW_AT_abstract_origin
	.byte	22                              # Abbrev [22] 0x1e2:0x7 DW_TAG_formal_parameter
	.byte	1                               # DW_AT_location
	.byte	85
	.long	770                             # DW_AT_abstract_origin
	.byte	22                              # Abbrev [22] 0x1e9:0x7 DW_TAG_formal_parameter
	.byte	1                               # DW_AT_location
	.byte	84
	.long	781                             # DW_AT_abstract_origin
	.byte	16                              # Abbrev [16] 0x1f0:0x9 DW_TAG_variable
	.long	.Ldebug_loc3                    # DW_AT_location
	.long	792                             # DW_AT_abstract_origin
	.byte	14                              # Abbrev [14] 0x1f9:0x22 DW_TAG_inlined_subroutine
	.long	432                             # DW_AT_abstract_origin
	.quad	.Ltmp16                         # DW_AT_low_pc
	.long	.Ltmp17-.Ltmp16                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	24                              # DW_AT_call_line
	.byte	5                               # DW_AT_call_column
	.byte	23                              # Abbrev [23] 0x20d:0x7 DW_TAG_formal_parameter
	.ascii	"\324\007"                      # DW_AT_const_value
	.long	440                             # DW_AT_abstract_origin
	.byte	23                              # Abbrev [23] 0x214:0x6 DW_TAG_formal_parameter
	.byte	14                              # DW_AT_const_value
	.long	451                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	14                              # Abbrev [14] 0x21b:0x23 DW_TAG_inlined_subroutine
	.long	432                             # DW_AT_abstract_origin
	.quad	.Ltmp18                         # DW_AT_low_pc
	.long	.Ltmp19-.Ltmp18                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	25                              # DW_AT_call_line
	.byte	5                               # DW_AT_call_column
	.byte	23                              # Abbrev [23] 0x22f:0x7 DW_TAG_formal_parameter
	.ascii	"\325\007"                      # DW_AT_const_value
	.long	440                             # DW_AT_abstract_origin
	.byte	22                              # Abbrev [22] 0x236:0x7 DW_TAG_formal_parameter
	.byte	1                               # DW_AT_location
	.byte	80
	.long	451                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	14                              # Abbrev [14] 0x23e:0x22 DW_TAG_inlined_subroutine
	.long	432                             # DW_AT_abstract_origin
	.quad	.Ltmp19                         # DW_AT_low_pc
	.long	.Ltmp21-.Ltmp19                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	26                              # DW_AT_call_line
	.byte	5                               # DW_AT_call_column
	.byte	23                              # Abbrev [23] 0x252:0x7 DW_TAG_formal_parameter
	.ascii	"\324\007"                      # DW_AT_const_value
	.long	440                             # DW_AT_abstract_origin
	.byte	23                              # Abbrev [23] 0x259:0x6 DW_TAG_formal_parameter
	.byte	15                              # DW_AT_const_value
	.long	451                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	14                              # Abbrev [14] 0x260:0x23 DW_TAG_inlined_subroutine
	.long	432                             # DW_AT_abstract_origin
	.quad	.Ltmp21                         # DW_AT_low_pc
	.long	.Ltmp22-.Ltmp21                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	27                              # DW_AT_call_line
	.byte	5                               # DW_AT_call_column
	.byte	23                              # Abbrev [23] 0x274:0x7 DW_TAG_formal_parameter
	.ascii	"\325\007"                      # DW_AT_const_value
	.long	440                             # DW_AT_abstract_origin
	.byte	22                              # Abbrev [22] 0x27b:0x7 DW_TAG_formal_parameter
	.byte	1                               # DW_AT_location
	.byte	82
	.long	451                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	18                              # Abbrev [18] 0x284:0x53 DW_TAG_subprogram
	.quad	.Lfunc_begin4                   # DW_AT_low_pc
	.long	.Lfunc_end4-.Lfunc_begin4       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
                                        # DW_AT_GNU_all_call_sites
	.long	1021                            # DW_AT_abstract_origin
	.byte	15                              # Abbrev [15] 0x297:0x2b DW_TAG_lexical_block
	.quad	.Ltmp24                         # DW_AT_low_pc
	.long	.Ltmp28-.Ltmp24                 # DW_AT_high_pc
	.byte	16                              # Abbrev [16] 0x2a4:0x9 DW_TAG_variable
	.long	.Ldebug_loc4                    # DW_AT_location
	.long	1030                            # DW_AT_abstract_origin
	.byte	15                              # Abbrev [15] 0x2ad:0x14 DW_TAG_lexical_block
	.quad	.Ltmp24                         # DW_AT_low_pc
	.long	.Ltmp25-.Ltmp24                 # DW_AT_high_pc
	.byte	17                              # Abbrev [17] 0x2ba:0x6 DW_TAG_variable
	.byte	0                               # DW_AT_const_value
	.long	1042                            # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	15                              # Abbrev [15] 0x2c2:0x14 DW_TAG_lexical_block
	.quad	.Ltmp29                         # DW_AT_low_pc
	.long	.Ltmp31-.Ltmp29                 # DW_AT_high_pc
	.byte	17                              # Abbrev [17] 0x2cf:0x6 DW_TAG_variable
	.byte	0                               # DW_AT_const_value
	.long	1056                            # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	24                              # Abbrev [24] 0x2d7:0x23 DW_TAG_subprogram
	.long	.Linfo_string21                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	10                              # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	110                             # DW_AT_type
	.byte	1                               # DW_AT_inline
	.byte	21                              # Abbrev [21] 0x2e3:0xb DW_TAG_formal_parameter
	.long	.Linfo_string22                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	10                              # DW_AT_decl_line
	.long	203                             # DW_AT_type
	.byte	21                              # Abbrev [21] 0x2ee:0xb DW_TAG_formal_parameter
	.long	.Linfo_string12                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	10                              # DW_AT_decl_line
	.long	191                             # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	10                              # Abbrev [10] 0x2fa:0x2a DW_TAG_subprogram
	.long	.Linfo_string23                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	22                              # DW_AT_decl_line
                                        # DW_AT_prototyped
                                        # DW_AT_external
	.byte	1                               # DW_AT_inline
	.byte	21                              # Abbrev [21] 0x302:0xb DW_TAG_formal_parameter
	.long	.Linfo_string17                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	22                              # DW_AT_decl_line
	.long	59                              # DW_AT_type
	.byte	21                              # Abbrev [21] 0x30d:0xb DW_TAG_formal_parameter
	.long	.Linfo_string16                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	22                              # DW_AT_decl_line
	.long	59                              # DW_AT_type
	.byte	12                              # Abbrev [12] 0x318:0xb DW_TAG_variable
	.long	.Linfo_string24                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	23                              # DW_AT_decl_line
	.long	110                             # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	13                              # Abbrev [13] 0x324:0xd9 DW_TAG_subprogram
	.quad	.Lfunc_begin5                   # DW_AT_low_pc
	.long	.Lfunc_end5-.Lfunc_begin5       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
                                        # DW_AT_GNU_all_call_sites
	.long	.Linfo_string33                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	46                              # DW_AT_decl_line
                                        # DW_AT_prototyped
                                        # DW_AT_external
	.byte	14                              # Abbrev [14] 0x339:0x1b DW_TAG_inlined_subroutine
	.long	727                             # DW_AT_abstract_origin
	.quad	.Ltmp35                         # DW_AT_low_pc
	.long	.Ltmp36-.Ltmp35                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	49                              # DW_AT_call_line
	.byte	45                              # DW_AT_call_column
	.byte	23                              # Abbrev [23] 0x34d:0x6 DW_TAG_formal_parameter
	.byte	32                              # DW_AT_const_value
	.long	739                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	14                              # Abbrev [14] 0x354:0xa8 DW_TAG_inlined_subroutine
	.long	762                             # DW_AT_abstract_origin
	.quad	.Ltmp37                         # DW_AT_low_pc
	.long	.Ltmp44-.Ltmp37                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	50                              # DW_AT_call_line
	.byte	9                               # DW_AT_call_column
	.byte	16                              # Abbrev [16] 0x368:0x9 DW_TAG_variable
	.long	.Ldebug_loc5                    # DW_AT_location
	.long	792                             # DW_AT_abstract_origin
	.byte	14                              # Abbrev [14] 0x371:0x22 DW_TAG_inlined_subroutine
	.long	432                             # DW_AT_abstract_origin
	.quad	.Ltmp38                         # DW_AT_low_pc
	.long	.Ltmp39-.Ltmp38                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	24                              # DW_AT_call_line
	.byte	5                               # DW_AT_call_column
	.byte	23                              # Abbrev [23] 0x385:0x7 DW_TAG_formal_parameter
	.ascii	"\324\007"                      # DW_AT_const_value
	.long	440                             # DW_AT_abstract_origin
	.byte	23                              # Abbrev [23] 0x38c:0x6 DW_TAG_formal_parameter
	.byte	14                              # DW_AT_const_value
	.long	451                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	14                              # Abbrev [14] 0x393:0x23 DW_TAG_inlined_subroutine
	.long	432                             # DW_AT_abstract_origin
	.quad	.Ltmp40                         # DW_AT_low_pc
	.long	.Ltmp41-.Ltmp40                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	25                              # DW_AT_call_line
	.byte	5                               # DW_AT_call_column
	.byte	23                              # Abbrev [23] 0x3a7:0x7 DW_TAG_formal_parameter
	.ascii	"\325\007"                      # DW_AT_const_value
	.long	440                             # DW_AT_abstract_origin
	.byte	22                              # Abbrev [22] 0x3ae:0x7 DW_TAG_formal_parameter
	.byte	1                               # DW_AT_location
	.byte	80
	.long	451                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	14                              # Abbrev [14] 0x3b6:0x22 DW_TAG_inlined_subroutine
	.long	432                             # DW_AT_abstract_origin
	.quad	.Ltmp41                         # DW_AT_low_pc
	.long	.Ltmp43-.Ltmp41                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	26                              # DW_AT_call_line
	.byte	5                               # DW_AT_call_column
	.byte	23                              # Abbrev [23] 0x3ca:0x7 DW_TAG_formal_parameter
	.ascii	"\324\007"                      # DW_AT_const_value
	.long	440                             # DW_AT_abstract_origin
	.byte	23                              # Abbrev [23] 0x3d1:0x6 DW_TAG_formal_parameter
	.byte	15                              # DW_AT_const_value
	.long	451                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	14                              # Abbrev [14] 0x3d8:0x23 DW_TAG_inlined_subroutine
	.long	432                             # DW_AT_abstract_origin
	.quad	.Ltmp43                         # DW_AT_low_pc
	.long	.Ltmp44-.Ltmp43                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	27                              # DW_AT_call_line
	.byte	5                               # DW_AT_call_column
	.byte	23                              # Abbrev [23] 0x3ec:0x7 DW_TAG_formal_parameter
	.ascii	"\325\007"                      # DW_AT_const_value
	.long	440                             # DW_AT_abstract_origin
	.byte	22                              # Abbrev [22] 0x3f3:0x7 DW_TAG_formal_parameter
	.byte	1                               # DW_AT_location
	.byte	82
	.long	451                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	10                              # Abbrev [10] 0x3fd:0x30 DW_TAG_subprogram
	.long	.Linfo_string25                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	37                              # DW_AT_decl_line
                                        # DW_AT_prototyped
                                        # DW_AT_external
	.byte	1                               # DW_AT_inline
	.byte	11                              # Abbrev [11] 0x405:0x1a DW_TAG_lexical_block
	.byte	12                              # Abbrev [12] 0x406:0xb DW_TAG_variable
	.long	.Linfo_string16                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	38                              # DW_AT_decl_line
	.long	59                              # DW_AT_type
	.byte	11                              # Abbrev [11] 0x411:0xd DW_TAG_lexical_block
	.byte	12                              # Abbrev [12] 0x412:0xb DW_TAG_variable
	.long	.Linfo_string17                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	39                              # DW_AT_decl_line
	.long	59                              # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	11                              # Abbrev [11] 0x41f:0xd DW_TAG_lexical_block
	.byte	12                              # Abbrev [12] 0x420:0xb DW_TAG_variable
	.long	.Linfo_string17                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	41                              # DW_AT_decl_line
	.long	59                              # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	13                              # Abbrev [13] 0x42d:0x16c DW_TAG_subprogram
	.quad	.Lfunc_begin6                   # DW_AT_low_pc
	.long	.Lfunc_end6-.Lfunc_begin6       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
                                        # DW_AT_GNU_all_call_sites
	.long	.Linfo_string34                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	53                              # DW_AT_decl_line
                                        # DW_AT_prototyped
                                        # DW_AT_external
	.byte	25                              # Abbrev [25] 0x442:0xf DW_TAG_formal_parameter
	.long	.Ldebug_loc6                    # DW_AT_location
	.long	.Linfo_string22                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	53                              # DW_AT_decl_line
	.long	1561                            # DW_AT_type
	.byte	14                              # Abbrev [14] 0x451:0x1c DW_TAG_inlined_subroutine
	.long	727                             # DW_AT_abstract_origin
	.quad	.Ltmp48                         # DW_AT_low_pc
	.long	.Ltmp49-.Ltmp48                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	64                              # DW_AT_call_line
	.byte	45                              # DW_AT_call_column
	.byte	22                              # Abbrev [22] 0x465:0x7 DW_TAG_formal_parameter
	.byte	1                               # DW_AT_location
	.byte	85
	.long	739                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	15                              # Abbrev [15] 0x46d:0x1d DW_TAG_lexical_block
	.quad	.Ltmp50                         # DW_AT_low_pc
	.long	.Ltmp62-.Ltmp50                 # DW_AT_high_pc
	.byte	26                              # Abbrev [26] 0x47a:0xf DW_TAG_variable
	.long	.Ldebug_loc7                    # DW_AT_location
	.long	.Linfo_string36                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	58                              # DW_AT_decl_line
	.long	59                              # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	14                              # Abbrev [14] 0x48a:0x54 DW_TAG_inlined_subroutine
	.long	1021                            # DW_AT_abstract_origin
	.quad	.Ltmp71                         # DW_AT_low_pc
	.long	.Ltmp79-.Ltmp71                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	72                              # DW_AT_call_line
	.byte	9                               # DW_AT_call_column
	.byte	15                              # Abbrev [15] 0x49e:0x2b DW_TAG_lexical_block
	.quad	.Ltmp71                         # DW_AT_low_pc
	.long	.Ltmp75-.Ltmp71                 # DW_AT_high_pc
	.byte	16                              # Abbrev [16] 0x4ab:0x9 DW_TAG_variable
	.long	.Ldebug_loc8                    # DW_AT_location
	.long	1030                            # DW_AT_abstract_origin
	.byte	15                              # Abbrev [15] 0x4b4:0x14 DW_TAG_lexical_block
	.quad	.Ltmp71                         # DW_AT_low_pc
	.long	.Ltmp72-.Ltmp71                 # DW_AT_high_pc
	.byte	17                              # Abbrev [17] 0x4c1:0x6 DW_TAG_variable
	.byte	0                               # DW_AT_const_value
	.long	1042                            # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	15                              # Abbrev [15] 0x4c9:0x14 DW_TAG_lexical_block
	.quad	.Ltmp76                         # DW_AT_low_pc
	.long	.Ltmp78-.Ltmp76                 # DW_AT_high_pc
	.byte	17                              # Abbrev [17] 0x4d6:0x6 DW_TAG_variable
	.byte	0                               # DW_AT_const_value
	.long	1056                            # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	14                              # Abbrev [14] 0x4de:0xba DW_TAG_inlined_subroutine
	.long	762                             # DW_AT_abstract_origin
	.quad	.Ltmp79                         # DW_AT_low_pc
	.long	.Ltmp88-.Ltmp79                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	73                              # DW_AT_call_line
	.byte	5                               # DW_AT_call_column
	.byte	27                              # Abbrev [27] 0x4f2:0x9 DW_TAG_formal_parameter
	.long	.Ldebug_loc9                    # DW_AT_location
	.long	770                             # DW_AT_abstract_origin
	.byte	27                              # Abbrev [27] 0x4fb:0x9 DW_TAG_formal_parameter
	.long	.Ldebug_loc10                   # DW_AT_location
	.long	781                             # DW_AT_abstract_origin
	.byte	16                              # Abbrev [16] 0x504:0x9 DW_TAG_variable
	.long	.Ldebug_loc11                   # DW_AT_location
	.long	792                             # DW_AT_abstract_origin
	.byte	14                              # Abbrev [14] 0x50d:0x22 DW_TAG_inlined_subroutine
	.long	432                             # DW_AT_abstract_origin
	.quad	.Ltmp81                         # DW_AT_low_pc
	.long	.Ltmp83-.Ltmp81                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	24                              # DW_AT_call_line
	.byte	5                               # DW_AT_call_column
	.byte	23                              # Abbrev [23] 0x521:0x7 DW_TAG_formal_parameter
	.ascii	"\324\007"                      # DW_AT_const_value
	.long	440                             # DW_AT_abstract_origin
	.byte	23                              # Abbrev [23] 0x528:0x6 DW_TAG_formal_parameter
	.byte	14                              # DW_AT_const_value
	.long	451                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	14                              # Abbrev [14] 0x52f:0x23 DW_TAG_inlined_subroutine
	.long	432                             # DW_AT_abstract_origin
	.quad	.Ltmp84                         # DW_AT_low_pc
	.long	.Ltmp85-.Ltmp84                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	25                              # DW_AT_call_line
	.byte	5                               # DW_AT_call_column
	.byte	23                              # Abbrev [23] 0x543:0x7 DW_TAG_formal_parameter
	.ascii	"\325\007"                      # DW_AT_const_value
	.long	440                             # DW_AT_abstract_origin
	.byte	22                              # Abbrev [22] 0x54a:0x7 DW_TAG_formal_parameter
	.byte	1                               # DW_AT_location
	.byte	80
	.long	451                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	14                              # Abbrev [14] 0x552:0x22 DW_TAG_inlined_subroutine
	.long	432                             # DW_AT_abstract_origin
	.quad	.Ltmp85                         # DW_AT_low_pc
	.long	.Ltmp87-.Ltmp85                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	26                              # DW_AT_call_line
	.byte	5                               # DW_AT_call_column
	.byte	23                              # Abbrev [23] 0x566:0x7 DW_TAG_formal_parameter
	.ascii	"\324\007"                      # DW_AT_const_value
	.long	440                             # DW_AT_abstract_origin
	.byte	23                              # Abbrev [23] 0x56d:0x6 DW_TAG_formal_parameter
	.byte	15                              # DW_AT_const_value
	.long	451                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	14                              # Abbrev [14] 0x574:0x23 DW_TAG_inlined_subroutine
	.long	432                             # DW_AT_abstract_origin
	.quad	.Ltmp87                         # DW_AT_low_pc
	.long	.Ltmp88-.Ltmp87                 # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	27                              # DW_AT_call_line
	.byte	5                               # DW_AT_call_column
	.byte	23                              # Abbrev [23] 0x588:0x7 DW_TAG_formal_parameter
	.ascii	"\325\007"                      # DW_AT_const_value
	.long	440                             # DW_AT_abstract_origin
	.byte	22                              # Abbrev [22] 0x58f:0x7 DW_TAG_formal_parameter
	.byte	1                               # DW_AT_location
	.byte	82
	.long	451                             # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	18                              # Abbrev [18] 0x599:0x4a DW_TAG_subprogram
	.quad	.Lfunc_begin7                   # DW_AT_low_pc
	.long	.Lfunc_end7-.Lfunc_begin7       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
                                        # DW_AT_GNU_all_call_sites
	.long	1507                            # DW_AT_abstract_origin
	.byte	27                              # Abbrev [27] 0x5ac:0x9 DW_TAG_formal_parameter
	.long	.Ldebug_loc12                   # DW_AT_location
	.long	1515                            # DW_AT_abstract_origin
	.byte	27                              # Abbrev [27] 0x5b5:0x9 DW_TAG_formal_parameter
	.long	.Ldebug_loc13                   # DW_AT_location
	.long	1526                            # DW_AT_abstract_origin
	.byte	15                              # Abbrev [15] 0x5be:0x17 DW_TAG_lexical_block
	.quad	.Lfunc_begin7                   # DW_AT_low_pc
	.long	.Ltmp96-.Lfunc_begin7           # DW_AT_high_pc
	.byte	16                              # Abbrev [16] 0x5cb:0x9 DW_TAG_variable
	.long	.Ldebug_loc14                   # DW_AT_location
	.long	1538                            # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	28                              # Abbrev [28] 0x5d5:0xd DW_TAG_GNU_call_site
	.long	1069                            # DW_AT_abstract_origin
	.quad	.Ltmp93                         # DW_AT_low_pc
	.byte	0                               # End Of Children Mark
	.byte	10                              # Abbrev [10] 0x5e3:0x2c DW_TAG_subprogram
	.long	.Linfo_string26                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	75                              # DW_AT_decl_line
                                        # DW_AT_prototyped
                                        # DW_AT_external
	.byte	1                               # DW_AT_inline
	.byte	21                              # Abbrev [21] 0x5eb:0xb DW_TAG_formal_parameter
	.long	.Linfo_string27                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	75                              # DW_AT_decl_line
	.long	1551                            # DW_AT_type
	.byte	21                              # Abbrev [21] 0x5f6:0xb DW_TAG_formal_parameter
	.long	.Linfo_string29                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	75                              # DW_AT_decl_line
	.long	59                              # DW_AT_type
	.byte	11                              # Abbrev [11] 0x601:0xd DW_TAG_lexical_block
	.byte	12                              # Abbrev [12] 0x602:0xb DW_TAG_variable
	.long	.Linfo_string30                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	76                              # DW_AT_decl_line
	.long	59                              # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	7                               # Abbrev [7] 0x60f:0x5 DW_TAG_pointer_type
	.long	1556                            # DW_AT_type
	.byte	3                               # Abbrev [3] 0x614:0x5 DW_TAG_const_type
	.long	1561                            # DW_AT_type
	.byte	5                               # Abbrev [5] 0x619:0x7 DW_TAG_base_type
	.long	.Linfo_string28                 # DW_AT_name
	.byte	6                               # DW_AT_encoding
	.byte	1                               # DW_AT_byte_size
	.byte	13                              # Abbrev [13] 0x620:0x74 DW_TAG_subprogram
	.quad	.Lfunc_begin8                   # DW_AT_low_pc
	.long	.Lfunc_end8-.Lfunc_begin8       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
                                        # DW_AT_GNU_all_call_sites
	.long	.Linfo_string35                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	79                              # DW_AT_decl_line
                                        # DW_AT_prototyped
                                        # DW_AT_external
	.byte	25                              # Abbrev [25] 0x635:0xf DW_TAG_formal_parameter
	.long	.Ldebug_loc15                   # DW_AT_location
	.long	.Linfo_string27                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	79                              # DW_AT_decl_line
	.long	1551                            # DW_AT_type
	.byte	29                              # Abbrev [29] 0x644:0xf DW_TAG_variable
	.byte	3                               # DW_AT_location
	.byte	126
	.byte	2
	.byte	159
	.long	.Linfo_string37                 # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	80                              # DW_AT_decl_line
	.long	59                              # DW_AT_type
	.byte	14                              # Abbrev [14] 0x653:0x33 DW_TAG_inlined_subroutine
	.long	1507                            # DW_AT_abstract_origin
	.quad	.Ltmp103                        # DW_AT_low_pc
	.long	.Ltmp110-.Ltmp103               # DW_AT_high_pc
	.byte	4                               # DW_AT_call_file
	.byte	82                              # DW_AT_call_line
	.byte	5                               # DW_AT_call_column
	.byte	22                              # Abbrev [22] 0x667:0x7 DW_TAG_formal_parameter
	.byte	1                               # DW_AT_location
	.byte	83
	.long	1515                            # DW_AT_abstract_origin
	.byte	15                              # Abbrev [15] 0x66e:0x17 DW_TAG_lexical_block
	.quad	.Ltmp103                        # DW_AT_low_pc
	.long	.Ltmp110-.Ltmp103               # DW_AT_high_pc
	.byte	16                              # Abbrev [16] 0x67b:0x9 DW_TAG_variable
	.long	.Ldebug_loc16                   # DW_AT_location
	.long	1538                            # DW_AT_abstract_origin
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	28                              # Abbrev [28] 0x686:0xd DW_TAG_GNU_call_site
	.long	1069                            # DW_AT_abstract_origin
	.quad	.Ltmp107                        # DW_AT_low_pc
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
.Ldebug_info_end0:
	.section	.debug_str,"MS",@progbits,1
.Linfo_string0:
	.asciz	"clang version 18.1.6 (https://github.com/ziglang/zig-bootstrap 98bc6bf4fc4009888d33941daf6b600d20a42a56)" # string offset=0
.Linfo_string1:
	.asciz	"C:\\Users\\Gower\\Downloads\\BojanOS\\src/vga.c" # string offset=105
.Linfo_string2:
	.asciz	"C:/Users/Gower"                # string offset=148
.Linfo_string3:
	.asciz	"VGA_WIDTH"                     # string offset=163
.Linfo_string4:
	.asciz	"unsigned long"                 # string offset=173
.Linfo_string5:
	.asciz	"size_t"                        # string offset=187
.Linfo_string6:
	.asciz	"VGA_HEIGHT"                    # string offset=194
.Linfo_string7:
	.asciz	"VGA_MEMORY"                    # string offset=205
.Linfo_string8:
	.asciz	"unsigned short"                # string offset=216
.Linfo_string9:
	.asciz	"uint16_t"                      # string offset=231
.Linfo_string10:
	.asciz	"row"                           # string offset=240
.Linfo_string11:
	.asciz	"col"                           # string offset=244
.Linfo_string12:
	.asciz	"color"                         # string offset=248
.Linfo_string13:
	.asciz	"unsigned char"                 # string offset=254
.Linfo_string14:
	.asciz	"uint8_t"                       # string offset=268
.Linfo_string15:
	.asciz	"vga_clear"                     # string offset=276
.Linfo_string16:
	.asciz	"y"                             # string offset=286
.Linfo_string17:
	.asciz	"x"                             # string offset=288
.Linfo_string18:
	.asciz	"outb"                          # string offset=290
.Linfo_string19:
	.asciz	"port"                          # string offset=295
.Linfo_string20:
	.asciz	"val"                           # string offset=300
.Linfo_string21:
	.asciz	"vga_entry"                     # string offset=304
.Linfo_string22:
	.asciz	"c"                             # string offset=314
.Linfo_string23:
	.asciz	"vga_setcursor"                 # string offset=316
.Linfo_string24:
	.asciz	"pos"                           # string offset=330
.Linfo_string25:
	.asciz	"vga_scroll"                    # string offset=334
.Linfo_string26:
	.asciz	"vga_write"                     # string offset=345
.Linfo_string27:
	.asciz	"data"                          # string offset=355
.Linfo_string28:
	.asciz	"char"                          # string offset=360
.Linfo_string29:
	.asciz	"size"                          # string offset=365
.Linfo_string30:
	.asciz	"i"                             # string offset=370
.Linfo_string31:
	.asciz	"vga_init"                      # string offset=372
.Linfo_string32:
	.asciz	"vga_setcolor"                  # string offset=381
.Linfo_string33:
	.asciz	"vga_backspace"                 # string offset=394
.Linfo_string34:
	.asciz	"vga_putchar"                   # string offset=408
.Linfo_string35:
	.asciz	"vga_print"                     # string offset=420
.Linfo_string36:
	.asciz	"next"                          # string offset=430
.Linfo_string37:
	.asciz	"len"                           # string offset=435
	.ident	"clang version 18.1.6 (https://github.com/ziglang/zig-bootstrap 98bc6bf4fc4009888d33941daf6b600d20a42a56)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.section	.debug_line,"",@progbits
.Lline_table_start0:
