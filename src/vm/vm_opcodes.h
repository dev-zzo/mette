#ifndef __mette_vm_opcodes_h_included
#define __mette_vm_opcodes_h_included

/*

Some random notes on the VM implementation here...
Taken bits from .NET VM, some from Forth...

The VM is tailored to execute code written in C-like language(s).

*** Calling subroutines/functions ***

All the args are passed via the stack, as are the return value(s).
Note it is possible to return multiple values from a subroutine.

*/

typedef enum {
	VMOP_ADD	= 0x00,	/* a b -- sum */
	VMOP_SUB	= 0x01,	/* a b -- diff */
	
	VMOP_UMUL	= 0x04,	/* a b -- hi lo */
	VMOP_SMUL	= 0x05,	/* a b -- hi lo */
	VMOP_UDIV	= 0x06,	/* a b -- mod div */
	VMOP_SDIV	= 0x07,	/* a b -- mod div */
	
	/* Bitwise operations */
	VMOP_AND	= 0x10,
	VMOP_OR		= 0x11,
	VMOP_XOR	= 0x12,
	VMOP_NOT	= 0x13,
	VMOP_LSL	= 0x18,
	VMOP_LSR	= 0x19,
	VMOP_ASR	= 0x1A,
	
	/* Conditionals */
	VMOP_CC_LT	= 0x40,
	VMOP_CC_GT	= 0x41,
	VMOP_CC_LE	= 0x42,
	VMOP_CC_GE	= 0x43,
	VMOP_CC_A	= 0x44,
	VMOP_CC_B	= 0x45,
	VMOP_CC_AE	= 0x46,
	VMOP_CC_BE	= 0x47,
	VMOP_CC_EQ	= 0x48,
	VMOP_CC_NE	= 0x49,
	
	/* Push a zero on the stack. */
	VMOP_LD0	= 0xC0,
	/* Push an one on the stack. */
	VMOP_LD1	= 0xC1,
	/* Push an unsigned 8-bit constant on the stack. */
	VMOP_LDU8	= 0xC8,
	/* Push a signed 8-bit constant on the stack. */
	VMOP_LDS8	= 0xC9,
	/* Push an unsigned 32-bit constant on the stack. */
	VMOP_LD32	= 0xCA,
	
	/* Push a value from the memory */
	VMOP_LDMU8	= 0xD0,
	VMOP_LDMS8	= 0xD1,
	VMOP_LDMU16	= 0xD2,
	VMOP_LDMS16 = 0xD3,
	VMOP_LDM32	= 0xD4,
	/* Pop a value into the memory */
	VMOP_STMU8	= 0xD8,
	VMOP_STMS8	= 0xD9,
	VMOP_STMU16	= 0xDA,
	VMOP_STMS16 = 0xDB,
	VMOP_STM32	= 0xDC,
	
	/* Duplicate the topmost stack entry. */
	VMOP_DUP	= 0xE0,
	/* Duplicate the nth stack entry. */
	VMOP_DUPV	= 0xE1,
	/* Swap the two topmost entries. */
	VMOP_SWAP	= 0xE2,
	/* Swap the topmost and nth entry. */
	VMOP_SWAPV	= 0xE3,
	/* Discard the topmost entry. */
	VMOP_POP	= 0xE4,
	
	/* Allocate host memory */
	VMOP_ALLOC	= 0xE8,
	/* Release host memory */
	VMOP_FREE	= 0xEC,
	
	/* Branch short unconditinally */
	VMOP_BRS	= 0xF0,
	/* Branch long unconditionally */
	VMOP_BRL	= 0xF1,
	/* Branch conditionally */
	VMOP_BRC	= 0xF4, /* if true (nonzero) */
	VMOP_BRNC	= 0xF5, /* if false (zero) */
	/* Call a VM subroutine */
	VMOP_CALL	= 0xF8,
	/* Return from a subroutine */
	VMOP_RET	= 0xFA,
	/* Call a native subroutine */
	VMOP_NCALL	= 0xFC,
} vm_opcode_t;

#endif // __mette_vm_opcodes_h_included

