.text
	.globl	__start
	.type	__start,@function
	.type	main,@function
	.ent	__start

__start:
	move	$31, $0

	lw		$4, 0($29) /* argc */
	addiu	$5, $29, 4 /* argv  */

	jal		main
	nop

	/* Exit */
	move	$4, $2
	li		$2, 4001 /* hardcoded... not good. */
	syscall

hlt:
	/* Hang.  */
	b   hlt

.size __start,.-__start
.end __start
