#include "vma.h"
#include <stdio.h>

static FILE *stream;

void vma_output_u8(uint8_t value)
{
	size_t count;
	count = fwrite(&value, 1, 1, stream);
	if (count != 1) {
		vma_abort("i/o failed");
	}
}

void vma_output_u16(uint16_t value)
{
	size_t count;
	/* TODO: endianness */
	count = fwrite(&value, 2, 1, stream);
	if (count != 1) {
		vma_abort("i/o failed");
	}
}

void vma_output_u32(uint32_t value)
{
	size_t count;
	/* TODO: endianness */
	count = fwrite(&value, 4, 1, stream);
	if (count != 1) {
		vma_abort("i/o failed");
	}
}

void vma_generate(struct vma_context *ctx)
{
	struct vma_insn_node *insn;

	VMA_ASSERT(ctx);
	VMA_ASSERT(ctx->output);
	VMA_ASSERT(ctx->unit);

	stream = ctx->output;

	vma_output_u8('B'); vma_output_u8('A'); vma_output_u8('R'); vma_output_u8('F');
	vma_output_u32(ctx->start_va);
	vma_output_u32(ctx->end_va - ctx->start_va);
	vma_output_u32(ctx->bss_va - ctx->start_va);

	insn = ctx->unit->head;
	while (insn && insn->start_addr < ctx->bss_va) {
		vma_output_insn(insn);
		insn = insn->next;
	}
}
