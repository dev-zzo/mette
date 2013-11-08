#include "imp_internal.h"

void imp_add(const struct imp_num_t *lo, const struct imp_num_t *ro, struct imp_num_t *res)
{
	char overflow = 0;
	__asm__ __volatile__ (
		"movl %1, %%ebx" "\n\t"
		"clc" "\n\t"
		"1:" "\n\t"
		"movl (%%esi), %%eax" "\n\t"
		"adcl (%%edi), %%eax" "\n\t"
		"movl %%eax, (%%edx)" "\n\t"
		"leal (%%ebx,%%edx,), %%edx" "\n\t"
		"leal (%%ebx,%%edi,), %%edi" "\n\t"
		"leal (%%ebx,%%esi,), %%esi" "\n\t"
		"loop 1b" "\n\t"
		"setc %%al" "\n\t"
		: "=a" (overflow)
		: "i" (sizeof(imp_archreg_t)), "S" (lo->bits), "D" (ro->bits), "d" (res->bits), "c" (lo->length / ARCH_REG_LENGTH)
		: "memory", "%ebx"
    );
}

