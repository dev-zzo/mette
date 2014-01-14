.text
	.globl	__start
	.type	__start,@function
	.type	_init,@function
	.type	_fini,@function
	.type	main,@function
	.type	__uClibc_main,@function
	.ent	__start

__start:
	la $28, _gp             /* Setup GP correctly if we're non-PIC.  */
	move $31, $0

	lw $5, 0($29)		/* argc */
	addiu $6, $29, 4	/* argv  */

	jal __uClibc_main

hlt:
	b   hlt
.size __start,.-__start
.end __start
