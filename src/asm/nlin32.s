	.file	"nlin32.c"
	.text
	.p2align 4,,15
.globl nlin32
	.type	nlin32, @function
nlin32:
	pushl	%ebp
	movl	%esp, %ebp
	movl	12(%ebp), %eax
	imull	8(%ebp), %eax
	addl	16(%ebp), %eax
	popl	%ebp
	ret
	.size	nlin32, .-nlin32
	.ident	"GCC: (Gentoo 4.3.2-r3 p1.6, pie-10.1.5) 4.3.2"
	.section	.note.GNU-stack,"",@progbits
