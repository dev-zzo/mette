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
	fputs("error: ", stderr);
	vfprintf(stderr, format, args);
	fputc('\n', stderr);
	va_end(args);

	if (++vma_errors > VMA_MAX_ERRORS) {
		vma_abort("too many errors to continue.");
	}
}

void vma_abort(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	fputs("fatal: ", stderr);
	vfprintf(stderr, format, args);
	fputc('\n', stderr);
	va_end(args);

	exit(1);
}

void vma_abort_on_errors(void)
{
	if (vma_errors) {
		vma_abort("errors encountered, aborting.");
	}
}

void vma_debug_print(const char *format, ...)
{
	va_list args;

	if (!vma_debug)
		return;

	va_start(args, format);
	fputs("debug: ", stderr);
	vfprintf(stderr, format, args);
	fputc('\n', stderr);
	va_end(args);
}

void vma_context_init(vma_context_t *ctx)
{
	ctx->output = ctx->input = NULL;
	ctx->start_symbol = "start";
	vma_symtab_init(&ctx->labels);
	vma_symtab_init(&ctx->ncalls);
	ctx->insns_tail = ctx->insns_head = NULL;
	ctx->start_va = 0;
	ctx->bss_va = 0;
	ctx->end_va = 0;
}