.text
	.globl	__start
	.type	__start,@function
	.type	main,@function

__start:
	xorl	%ebp, %ebp
	popl	%esi		/* Pop the argument count.  */
	movl	%esp, %ecx		/* argv starts just at the current stack top.*/

	pushl	%ecx
	pushl	%esi
	call	main

	/* Exit */
	movl	$1, %eax
	int		$80

hlt:
	/* Hang.  */
	jmp		hlt

.size __start,.-__start
.end __start
