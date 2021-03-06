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
	vma_symbol_t *ep_sym;
	vma_insn_t *insn;
	unsigned size_in_memory;
	unsigned size_in_file;
	unsigned ep_offset;

	VMA_ASSERT(ctx);
	VMA_ASSERT(ctx->output);

	stream = ctx->output;

	size_in_memory = ctx->end_va - ctx->start_va;
	size_in_file = ctx->bss_va - ctx->start_va;
	vma_debug_print("memory: %08X file: %08X", size_in_memory, size_in_file);

	ep_sym = vma_symtab_lookup(&ctx->globals, ctx->start_symbol);
	if (!ep_sym) {
		vma_abort("start symbol '%s' not defined", ctx->start_symbol);
	}
	ep_offset = ep_sym->u.location->start_addr - ctx->start_va;

	vma_output_u8('B'); vma_output_u8('A'); vma_output_u8('R'); vma_output_u8('F');
	vma_output_u32(size_in_memory); /* in memory */
	vma_output_u32(size_in_file); /* in file */
	vma_output_u32(ep_offset);
	vma_output_u16(ctx->ncalls.count);
	vma_output_u16(0xFFFF);
	vma_output_u32(0xFFFF);
	vma_output_u32(0xFFFF);
	vma_output_u32(0xFFFF);

	insn = ctx->insns_head;
	while (insn && insn->start_addr < ctx->bss_va) {
		vma_emit_insn(ctx, insn);
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
