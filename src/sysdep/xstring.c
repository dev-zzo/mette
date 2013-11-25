#include "xstring.h"

void *xmemcpy(void *dest, const void *src, size_t count)
{
	const char *s = src;
	char *d = dest;
	
	while (count--)
		*d++ = *s++;

	return dest;
}

void *xmemset(void *dest, int c, size_t count)
{
	char *d = dest;
	
	while (count--)
		*d++ = (char)c;

	return dest;
}

