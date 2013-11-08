#include "imp_internal.h"
#include <stdio.h>
#include <string.h>

void imp_num_load1(struct imp_num_t *num, imp_archreg_t value)
{
	memset(num->bits, 0, num->length / 8);
	num->bits[0] = value;
}

void imp_num_print(struct imp_num_t *num)
{
	imp_archreg_t *ptr = num->bits + (num->length / ARCH_REG_LENGTH);
	do {
		--ptr;
		printf("%08X ", *ptr);
	} while(ptr != num->bits);
}

