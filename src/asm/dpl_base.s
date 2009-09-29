	.file	"dpl_base.c"
	.text
	.p2align 4,,15
.globl _zeromem
	.type	_zeromem, @function
_zeromem:
	pushl	%ebp
	movl	%esp, %ebp
	testl	%edx, %edx
	je	.L4
	xorl	%eax, %eax
	.p2align 4,,15
.L3:
	movb	$0, (%ecx,%eax)
	incl	%eax
	cmpl	%edx, %eax
	jne	.L3
.L4:
	popl	%ebp
	ret
	.size	_zeromem, .-_zeromem
	.ident	"GCC: (Gentoo 4.3.2-r3 p1.6, pie-10.1.5) 4.3.2"
	.section	.note.GNU-stack,"",@progbits
