	.file	"sp2n32.c"
	.text
	.p2align 4,,15
.globl sp2n32
	.type	sp2n32, @function
sp2n32:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	popl	%ebp
	ret
	.size	sp2n32, .-sp2n32
	.ident	"GCC: (Gentoo 4.3.2-r3 p1.6, pie-10.1.5) 4.3.2"
	.section	.note.GNU-stack,"",@progbits
