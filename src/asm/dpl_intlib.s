	.file	"dpl_intlib.c"
	.text
	.p2align 4,,15
.globl nlp32
	.type	nlp32, @function
nlp32:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %edx
	movl	%edx, %eax
	shrl	$1, %eax
	orl	%edx, %eax
	movl	%eax, %edx
	shrl	$2, %edx
	orl	%eax, %edx
	movl	%edx, %eax
	shrl	$4, %eax
	orl	%edx, %eax
	movl	%eax, %edx
	shrl	$8, %edx
	orl	%eax, %edx
	movl	%edx, %eax
	shrl	$16, %eax
	orl	%edx, %eax
	incl	%eax
	popl	%ebp
	ret
	.size	nlp32, .-nlp32
	.p2align 4,,15
.globl msb32
	.type	msb32, @function
msb32:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %edx
	movl	%edx, %eax
	shrl	$1, %eax
	orl	%edx, %eax
	movl	%eax, %edx
	shrl	$2, %edx
	orl	%eax, %edx
	movl	%edx, %eax
	shrl	$4, %eax
	orl	%edx, %eax
	movl	%eax, %edx
	shrl	$8, %edx
	orl	%eax, %edx
	movl	%edx, %ecx
	shrl	$16, %ecx
	orl	%edx, %ecx
	movl	%ecx, %eax
	shrl	$1, %eax
	notl	%eax
	andl	%ecx, %eax
	popl	%ebp
	ret
	.size	msb32, .-msb32
	.p2align 4,,15
.globl one32
	.type	one32, @function
one32:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %edx
	movl	%edx, %eax
	shrl	$1, %eax
	andl	$1431655765, %eax
	subl	%eax, %edx
	movl	%edx, %ecx
	shrl	$2, %ecx
	andl	$858993459, %ecx
	andl	$858993459, %edx
	addl	%edx, %ecx
	movl	%ecx, %eax
	shrl	$4, %eax
	addl	%ecx, %eax
	andl	$252645135, %eax
	movl	%eax, %edx
	shrl	$8, %edx
	addl	%eax, %edx
	movl	%edx, %eax
	shrl	$16, %eax
	addl	%edx, %eax
	andl	$63, %eax
	popl	%ebp
	ret
	.size	one32, .-one32
	.p2align 4,,15
.globl abs32
	.type	abs32, @function
abs32:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	negl	%eax
	popl	%ebp
	ret
	.size	abs32, .-abs32
	.p2align 4,,15
.globl swap32
	.type	swap32, @function
swap32:
	pushl	%ebp
	movl	%esp, %ebp
	movl	(%edx), %eax
	xorl	(%ecx), %eax
	movl	%eax, (%ecx)
	xorl	(%edx), %eax
	movl	%eax, (%edx)
	xorl	%eax, (%ecx)
	popl	%ebp
	ret
	.size	swap32, .-swap32
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
	.p2align 4,,15
.globl zeroint
	.type	zeroint, @function
zeroint:
	pushl	%ebp
	movl	%esp, %ebp
	testl	%edx, %edx
	je	.L17
	.p2align 4,,15
.L18:
	movl	$0, (%ecx)
	addl	$4, %ecx
	decl	%edx
	jne	.L18
.L17:
	popl	%ebp
	ret
	.size	zeroint, .-zeroint
	.ident	"GCC: (Gentoo 4.3.2-r3 p1.6, pie-10.1.5) 4.3.2"
	.section	.note.GNU-stack,"",@progbits
