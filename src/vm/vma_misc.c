#include "vma.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define VMA_MAX_ERRORS 20

int vma_errors = 0;

void *vma_malloc(size_t count)
{
	void *result = malloc(count);
	if (!result) {
		vma_abort("out of memory");
	}
	return result;
}

void vma_free(void *ptr)
{
	free(ptr);
}

void vma_error(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);

	if (++vma_errors > VMA_MAX_ERRORS) {
		vma_abort("error: too many errors, aborting.");
	}
}

void vma_abort(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);

	exit(1);
}
