	.file	"main2.c"
	.local	A
	.comm	A,4,4
	.local	B
	.comm	B,4,4
	.local	C
	.comm	C,4,4
	.local	D
	.comm	D,4,4
	.local	R
	.comm	R,4,4
	.text
	.globl	process
	.type	process, @function
process:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	.cfi_offset 3, -12
	call	__x86.get_pc_thunk.ax
	addl	$_GLOBAL_OFFSET_TABLE_, %eax
	movl	A@GOTOFF(%eax), %ecx
	movl	B@GOTOFF(%eax), %edx
	imull	%edx, %ecx
	movl	C@GOTOFF(%eax), %ebx
	movl	D@GOTOFF(%eax), %edx
	imull	%ebx, %edx
	addl	%ecx, %edx
	movl	%edx, R@GOTOFF(%eax)
	nop
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	process, .-process
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	call	__x86.get_pc_thunk.ax
	addl	$_GLOBAL_OFFSET_TABLE_, %eax
	call	process
	movl	$0, %eax
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.section	.text.__x86.get_pc_thunk.ax,"axG",@progbits,__x86.get_pc_thunk.ax,comdat
	.globl	__x86.get_pc_thunk.ax
	.hidden	__x86.get_pc_thunk.ax
	.type	__x86.get_pc_thunk.ax, @function
__x86.get_pc_thunk.ax:
.LFB2:
	.cfi_startproc
	movl	(%esp), %eax
	ret
	.cfi_endproc
.LFE2:
	.ident	"GCC: (Debian 7.2.0-19) 7.2.0"
	.section	.note.GNU-stack,"",@progbits
