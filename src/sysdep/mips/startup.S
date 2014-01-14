.text
	.globl	__start
	.type	__start,@function
	.type	main,@function
	.ent	__start

__start:
	la		$28, _gp
	move	$31, $0

	lw		$4, 0($29) /* argc */
	addiu	$5, $29, 4 /* argv  */

	jal		main

	/* Exit */
	li		$4, 144
	li		$2, 1
	syscall
.size __start,.-__start
.end __start
