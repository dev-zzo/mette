#include "imp_internal.h"
#include <malloc.h>
#include <string.h>

void imp_num_init(struct imp_num_t *num, unsigned int length)
{
	num->bits = malloc(length / 8);
	num->length = length;
	memset(num->bits, 0, length / 8);
}

void imp_num_free(struct imp_num_t *num)
{
	free(num->bits);
}

