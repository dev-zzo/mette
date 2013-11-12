#include "imp_internal.h"

void imp_sub(const struct imp_num_t *lo, const struct imp_num_t *ro, struct imp_num_t *res)
{
	char underflow = 0;
	__asm__ __volatile__ (
		"clc" "\n\t"
		"pushf" "\n\t"
		"1:" "\n\t"
		"movl (%%esi), %%eax" "\n\t"
		"popf" "\n\t"
		"sbbl (%%edi), %%eax" "\n\t"
		"pushf" "\n\t"
		"movl %%eax, (%%edx)" "\n\t"
		"addl %1, %%edx" "\n\t"
		"addl %1, %%edi" "\n\t"
		"addl %1, %%esi" "\n\t"
		"loop 1b" "\n\t"
		"popf" "\n\t"
		"setc %%al" "\n\t"
		: "=a" (underflow)
		: "i" (sizeof(imp_archreg_t)), "S" (lo->bits), "D" (ro->bits), "d" (res->bits), "c" (lo->length / ARCH_REG_LENGTH_BITS)
		: "memory"
    );
}

