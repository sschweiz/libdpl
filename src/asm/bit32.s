	.file	"bit32.c"
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
	.ident	"GCC: (Gentoo 4.3.2-r3 p1.6, pie-10.1.5) 4.3.2"
	.section	.note.GNU-stack,"",@progbits
