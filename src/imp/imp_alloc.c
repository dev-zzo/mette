#include "imp_internal.h"
#include <malloc.h>
#include <string.h>

void imp_num_init(struct imp_num_t *num, unsigned int length)
{
	size_t length_bytes = ROUND_MASK(length, ARCH_REG_LENGTH_BITS - 1);
	
	num->bits = malloc(length_bytes);
	num->length = length;
	memset(num->bits, 0, length_bytes);
}

void imp_num_free(struct imp_num_t *num)
{
	free(num->bits);
}

