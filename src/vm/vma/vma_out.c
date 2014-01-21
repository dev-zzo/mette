#include "vma.h"
#include <stdio.h>
#include <stdint.h>

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

static uint32_t vma_hash_name(const char *name)
{
	uint32_t hash = 0;
	const char *ptr = name;

	while (*ptr) {
		hash = (hash * 61 + *ptr) ^ (hash >> 16);
		ptr++;
	}

	return hash;
}

void vma_generate(vma_context_t *ctx)
{
	vma_symbol_t *sym_ncall;
	vma_insn_t *insn;
	unsigned size_in_memory;
	unsigned size_in_file;

	VMA_ASSERT(ctx);
	VMA_ASSERT(ctx->output);

	stream = ctx->output;

	size_in_memory = ctx->end_va - ctx->start_va;
	size_in_memory = VMA_ALIGN(size_in_memory, 16) / 16;
	size_in_file = ctx->bss_va - ctx->start_va;
	size_in_file = VMA_ALIGN(size_in_file, 16) / 16;
	vma_debug_print("memory: %08X/%08X file: %08X/%08X",
		ctx->end_va - ctx->start_va, size_in_memory * 16,
		ctx->bss_va - ctx->start_va, size_in_file * 16);
	if (size_in_memory > 0xFFFF || size_in_file > 0xFFFF) {
		vma_abort("resulting code does not fit into image");
	}

	vma_output_u8('B'); vma_output_u8('A'); vma_output_u8('R'); vma_output_u8('F');
	vma_output_u32(ctx->start_va);
	vma_output_u16(size_in_memory); /* in memory */
	vma_output_u16(size_in_file); /* in file */
	vma_output_u16(ctx->ncalls.count);
	vma_output_u16(0xFFFF);

	insn = ctx->insns_head;
	while (insn && insn->start_addr < ctx->bss_va) {
		vma_insn_emit(insn);
		insn = insn->next;
	}

	sym_ncall = ctx->ncalls.head;
	while (sym_ncall) {
		uint32_t hash = vma_hash_name(sym_ncall->name);
		vma_output_u32(hash);
		vma_debug_print("ncall entry: %04X %08X %s", sym_ncall->u.id, hash, sym_ncall->name);
		sym_ncall = sym_ncall->next;
	}
}
