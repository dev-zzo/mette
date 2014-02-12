#include "vma.h"
#include <stdint.h>
#include <string.h>

/*******************************************************************************
 * Symbols
 */

void vma_symtab_init(vma_symtab_t *symtab)
{
	memset(symtab, 0, sizeof(*symtab));
}

vma_symbol_t *vma_symtab_define(vma_symtab_t *symtab, const char *name, vma_symbol_type_t type, vma_symbol_t **prev_def)
{
	vma_symbol_t *sym = vma_symtab_lookup(symtab, name);
	if (sym) {
		if (prev_def) {
			*prev_def = sym;
		}
		return sym;
	}

	if (prev_def) {
		*prev_def = NULL;
	}
	sym = (vma_symbol_t *)vma_malloc(sizeof(*sym));
	memset(sym, 0, sizeof(*sym));

	sym->name = name;
	sym->type = type;
	if (type == SYM_NCALL) {
		sym->u.id = symtab->count;
	}
	symtab->count++;
	if (symtab->tail) {
		symtab->tail->next = sym;
	} else {
		symtab->head = sym;
	}
	symtab->tail = sym;

	vma_debug_print("symtab %p: new symbol: `%s' (%p)", symtab, name, sym);

	return sym;
}

vma_symbol_t *vma_symtab_lookup(const vma_symtab_t *symtab, const char *name)
{
	vma_symbol_t *sym = symtab->head;

	while (sym) {
		if (!strcmp(sym->name, name)) {
			break;
		}
		sym = sym->next;
	}

	return sym;
}

vma_symbol_t *vma_symtab_lookup_chain(const vma_symtab_t *head, const char *name)
{
	while (head) {
		vma_symbol_t *sym = vma_symtab_lookup(head, name);
		if (sym) {
			return sym;
		}
		head = head->lookup_next;
	}

	return NULL;
}

void vma_symtab_dump(const vma_symtab_t *symtab, int resolved)
{
	vma_symbol_t *sym = symtab->head;

	vma_debug_print("dumping symtab %p", symtab);
	while (sym) {
		if (resolved) {
			vma_debug_print("%p %08X %s", sym, sym->u.location->start_addr, sym->name);
		} else {
			vma_debug_print("%p %8d %s", sym, sym->u.id, sym->name);
		}
		sym = sym->next;
	}
}

void vma_symref_init(vma_symref_t *ref, const char *name)
{
	ref->u.name = name;
}

int vma_symref_resolve(vma_symref_t *ref, vma_symtab_t *symtab)
{
	vma_symbol_t *sym;

	VMA_ASSERT(ref->u.name);

	sym = vma_symtab_lookup_chain(symtab, ref->u.name);
	if (!sym) {
		return 0;		
	}
	vma_debug_print("symref %p: symbol: `%s' -> %p (insn %p)", ref, ref->u.name, sym, sym->u.location);

	ref->u.sym = sym;
	return 1;
}

/*******************************************************************************
 * Expressions
 */

vma_expr_t *vma_expr_build_literal(int value)
{
	vma_expr_t *node = (vma_expr_t *)vma_malloc(sizeof(*node));

	node->next = NULL;
	node->type = EXPR_LITERAL;
	node->value = value;

	return node;
}

vma_expr_t *vma_expr_build_symref(const char *name)
{
	vma_expr_t *node = (vma_expr_t *)vma_malloc(sizeof(*node));

	node->next = NULL;
	node->type = EXPR_SYMREF;
	vma_symref_init(&node->u.symref, name);

	return node;
}

vma_expr_t *vma_expr_build_parent(vma_expr_type_t type, vma_expr_t *a, vma_expr_t *b)
{
	vma_expr_t *node = (vma_expr_t *)vma_malloc(sizeof(*node));

	node->next = NULL;
	node->type = type;
	node->u.child[0] = a;
	node->u.child[1] = b;

	return node;
}

/*******************************************************************************
 * Expression lists
 */

vma_expr_list_t *vma_expr_list_create(void)
{
	vma_expr_list_t *list = (vma_expr_list_t *)vma_malloc(sizeof(*list));
	list->tail = list->head = NULL;
	list->count = 0;
	return list;
}

vma_expr_list_t *vma_expr_list_append(vma_expr_list_t *list, vma_expr_t *node)
{
	node->next = NULL;
	if (list->tail) {
		list->tail->next = node;
	} else {
		list->head = node;
	}
	list->tail = node;
	list->count++;
	return list;
}

/*******************************************************************************
 * Instructions
 */

vma_insn_t *vma_insn_build(vma_insn_type_t type)
{
	vma_insn_t *insn = (vma_insn_t *)vma_malloc(sizeof(*insn));

	memset(insn, 0, sizeof(*insn));
	insn->type = type;

	return insn;
}

/*******************************************************************************
 * Assembly pass 1 (estimate insn sizes and allocate addresses)
 */

static int vma_passx_evaluate_expr(vma_context_t *ctx, vma_expr_t *expr);

static int vma_passx_evaluate_symref(vma_context_t *ctx, vma_symref_t *symref, uint32_t *value)
{
	vma_symbol_t *sym;
	vma_insn_t *label_target;

	if (!vma_symref_resolve(symref, ctx->lookup_stack)) {
		vma_error("unresolved symbol: `%s'.", symref->u.name);
		return 0;
	}

	sym = symref->u.sym;
	VMA_ASSERT(sym);

	switch (sym->type) {
		case SYM_LABEL:
			label_target = sym->u.location;
			if (label_target->flags.allocated) {
				*value = label_target->start_addr;
				return 1;
			}
			vma_error("referring to symbol `%s' before its location is known.", sym->name);
			break;

		case SYM_CONSTANT:
			/* TODO: don't evaluate this multiple times. */
			if (vma_passx_evaluate_expr(ctx, sym->u.value)) {
				*value = sym->u.value->value;
				return 1;
			}
			break;

		case SYM_NCALL:
			vma_error("referring to NCALL-defined symbol `%s' outside of NCALL.", sym->name);
			break;

		default:
			vma_abort("%s:%d: internal error: unhandled symbol type %d", __FILE__, __LINE__, sym->type);
			break;
	}
	return 0;
}

static int vma_passx_evaluate_expr(vma_context_t *ctx, vma_expr_t *expr)
{
	uint32_t lhs, rhs;

	VMA_ASSERT(expr);

	vma_debug_print("evaluating expr: %p", expr);
	switch (expr->type) {
		case EXPR_LITERAL:
			/* Already in expr->value. */
			return 1;

		case EXPR_SYMREF:
			return vma_passx_evaluate_symref(ctx, &expr->u.symref, &expr->value);

		case EXPR_OR:
		case EXPR_AND:
		case EXPR_XOR:
		case EXPR_ADD:
		case EXPR_SUB:
		case EXPR_MUL:
		case EXPR_DIV:
			if (!vma_passx_evaluate_expr(ctx, expr->u.child[0])) {
				return 0;
			}
			if (!vma_passx_evaluate_expr(ctx, expr->u.child[1])) {
				return 0;
			}

			lhs = expr->u.child[0]->value;
			rhs = expr->u.child[1]->value;

			switch (expr->type) {
				case EXPR_OR:
					expr->value = lhs | rhs;
					break;
				case EXPR_AND:
					expr->value = lhs & rhs;
					break;
				case EXPR_XOR:
					expr->value = lhs ^ rhs;
					break;
				case EXPR_ADD:
					expr->value = lhs + rhs;
					break;
				case EXPR_SUB:
					expr->value = lhs - rhs;
					break;
				case EXPR_MUL:
					expr->value = lhs * rhs;
					break;
				case EXPR_DIV:
					if (rhs == 0) {
						/* TODO: location tracking. */
						vma_error("divisor evaluates to zero");
						expr->value = 1;
					} else {
						expr->value = lhs / rhs;
					}
					break;
			}
			break;

		case EXPR_NEG:
		case EXPR_NOT:
			if (!vma_passx_evaluate_expr(ctx, expr->u.child[0])) {
				return 0;
			}

			rhs = expr->u.child[0]->value;

			switch (expr->type) {
				case EXPR_NEG:
					expr->value = -rhs;
					break;
				case EXPR_NOT:
					expr->value = ~rhs;
					break;
			}
			return 1;

		default:
			vma_abort("%s:%d: internal error: unhandled expr type %d", __FILE__, __LINE__, expr->type);
			return 0;
	}
	return 0;
}

static unsigned vma_pass1_insn_estimate_size(vma_context_t *ctx, vma_insn_t *insn)
{
	switch (insn->type) {
		case INSN_CONST:
		case INSN_SUBSTART:
		case INSN_SUBEND:
			return 0;

		case INSN_ADD:
		case INSN_SUB:
		case INSN_MULU:
		case INSN_MULS:
		case INSN_DIVU:
		case INSN_DIVS:
		case INSN_AND:
		case INSN_OR:
		case INSN_XOR:
		case INSN_NOT:
		case INSN_LSL:
		case INSN_LSR:
		case INSN_ASR:
		case INSN_CMP_LT:
		case INSN_CMP_GT:
		case INSN_CMP_B:
		case INSN_CMP_A:
		case INSN_CMP_EQ:
		case INSN_LDC_0:
		case INSN_LDC_1:
		case INSN_LDC_2:
		case INSN_LDM_UB:
		case INSN_LDM_SB:
		case INSN_LDM_UH:
		case INSN_LDM_SH:
		case INSN_LDM_W:
		case INSN_STM_B:
		case INSN_STM_H:
		case INSN_STM_W:
		case INSN_DUP:
		case INSN_SWAP:
		case INSN_POP:
		case INSN_RET:
		case INSN_ICALL:
		case INSN_IJMP:
			return 1;

		case INSN_LDC_UB:
		case INSN_LDC_SB:
		case INSN_LOCALS:
		case INSN_LDLOC:
		case INSN_STLOC:
		case INSN_BR:
		case INSN_BR_T:
		case INSN_BR_F:
			return 1 + 1;

		case INSN_NCALL:
			return 1 + 2;
			
		case INSN_LDC_W:
		case INSN_LEA:
		case INSN_CALL:
			return 1 + 4;
			
		case INSN_DEFS:
			VMA_ASSERT(insn->u.text.buffer);
			return insn->u.text.length;

		case INSN_DEFB:
			VMA_ASSERT(insn->u.expr_list);
			return sizeof(uint8_t) * insn->u.expr_list->count;

		case INSN_DEFH:
			VMA_ASSERT(insn->u.expr_list);
			return sizeof(uint16_t) * insn->u.expr_list->count;

		case INSN_DEFW:
			VMA_ASSERT(insn->u.expr_list);
			return sizeof(uint32_t) * insn->u.expr_list->count;

		case INSN_RESB:
			VMA_ASSERT(insn->u.expr);
			if (!vma_passx_evaluate_expr(ctx, insn->u.expr)) {
				vma_error("cannot estimate amount of memory for .RESx keyword.");
				return 0;
			}
			return sizeof(uint8_t) * insn->u.expr->value;

		case INSN_RESH:
			VMA_ASSERT(insn->u.expr);
			if (!vma_passx_evaluate_expr(ctx, insn->u.expr)) {
				vma_error("cannot estimate amount of memory for .RESx keyword.");
				return 0;
			}
			return sizeof(uint16_t) * insn->u.expr->value;

		case INSN_RESW:
			VMA_ASSERT(insn->u.expr);
			if (!vma_passx_evaluate_expr(ctx, insn->u.expr)) {
				vma_error("cannot estimate amount of memory for .RESx keyword.");
				return 0;
			}
			return sizeof(uint32_t) * insn->u.expr->value;

		default:
			vma_abort("%s:%d:internal error: unhandled insn type %d", __FILE__, __LINE__, insn->type);
			return 0;
	}
}

static void vma_pass1(vma_context_t *ctx)
{
	vma_insn_t *insn = ctx->insns_head;
	vma_vaddr_t next_va = ctx->start_va;
	vma_vaddr_t bss_va = ctx->start_va;

	vma_debug_print("running pass1...");
	while (insn) {

		switch (insn->type) {
			case INSN_DEFH:
			case INSN_RESH:
				insn->align_bytes = VMA_ALIGN(next_va, 2) - next_va;
				next_va = VMA_ALIGN(next_va, 2);
				break;

			case INSN_DEFW:
			case INSN_RESW:
				insn->align_bytes = VMA_ALIGN(next_va, 4) - next_va;
				next_va = VMA_ALIGN(next_va, 4);
				break;

			case INSN_SUBSTART:
				/* A bit ugly, but... */
				ctx->lookup_stack = &insn->u.symtab;
				break;

			case INSN_SUBEND:
				/* A bit ugly, but... */
				ctx->lookup_stack = &ctx->globals;
				break;
		}

		ctx->current_va = next_va;

		insn->start_addr = next_va;
		insn->flags.allocated = 1;
		next_va += vma_pass1_insn_estimate_size(ctx, insn);

		if (insn->type < INSN_ALLOCATE) {
			bss_va = next_va;
		}

		insn = insn->next;
	}

	ctx->end_va = next_va;
	ctx->bss_va = bss_va;

	vma_abort_on_errors();
}

/*******************************************************************************
 * Assembly pass 2 (evaluate and emit insn bits)
 */

#include "vm_opcodes.codes.tab"

void vma_emit_insn(vma_context_t *ctx, vma_insn_t *insn)
{
	int32_t diff;
	uint32_t count;
	vma_expr_t *expr;

	VMA_ASSERT(insn);

	if (insn->type < INSN_ASM_KEYWORDS) {
		vma_output_u8(vm_insn_to_opcode[insn->type]);
	}

	switch (insn->type) {
		case INSN_DEFH:
		case INSN_RESH:
		case INSN_DEFW:
		case INSN_RESW:
			count = insn->align_bytes;
			while (count--) {
				vma_output_u8(0xAA);
			}
			break;
	}

	switch (insn->type) {
		case INSN_SUBSTART:
			/* A bit ugly, but... */
			ctx->lookup_stack = &insn->u.symtab;
			break;

		case INSN_SUBEND:
			/* A bit ugly, but... */
			ctx->lookup_stack = &ctx->globals;
			break;

		case INSN_LDC_UB:
		case INSN_LDC_SB:
		case INSN_LOCALS:
		case INSN_LDLOC:
		case INSN_STLOC:
			vma_passx_evaluate_expr(ctx, insn->u.expr);
			if (insn->u.expr->value > 0xFF) {
				vma_error("line %d: expression value out of bounds (0x%08X, max 0xFF).", insn->line, insn->u.expr->value);
				break;
			}
			vma_output_u8((uint8_t)insn->u.expr->value);
			break;

		case INSN_BR:
		case INSN_BR_T:
		case INSN_BR_F:
			/* Relative to insn end. */
			vma_passx_evaluate_expr(ctx, insn->u.expr);
			diff = insn->u.expr->value - (insn->start_addr + 2); /* hard-coded :( */
			if (diff > 127 || diff < -128) {
				vma_error("line %d: branch target out of range (%+d bytes).", insn->line, diff);
			}
			vma_output_u8((uint8_t)diff);
			break;

		case INSN_NCALL:
			VMA_ASSERT(insn->u.symref.u.sym);
			vma_output_u16(insn->u.symref.u.sym->u.id);
			break;
			
		case INSN_LDC_W:
			vma_passx_evaluate_expr(ctx, insn->u.expr);
			vma_output_u32(insn->u.expr->value);
			break;

		case INSN_LEA:
		case INSN_CALL:
			/* Relative to insn end. */
			vma_passx_evaluate_expr(ctx, insn->u.expr);
			diff = insn->u.expr->value - (insn->start_addr + 5); /* hard-coded :( */
			vma_output_u32(diff);
			break;
			
		case INSN_DEFS: {
				const char *text = insn->u.text.buffer;
				unsigned count = insn->u.text.length;
				while (count) {
					vma_output_u8(*text);
					++text;
					--count;
				}
			}
			break;

		case INSN_DEFB:
			expr = insn->u.expr_list->head;
			while (expr) {
				vma_passx_evaluate_expr(ctx, expr);
				vma_output_u8((uint8_t)expr->value);
				expr = expr->next;
			}
			break;

		case INSN_DEFH:
			expr = insn->u.expr_list->head;
			while (expr) {
				vma_passx_evaluate_expr(ctx, expr);
				vma_output_u16((uint16_t)expr->value);
				expr = expr->next;
			}
			break;

		case INSN_DEFW:
			expr = insn->u.expr_list->head;
			while (expr) {
				vma_passx_evaluate_expr(ctx, expr);
				vma_output_u32((uint32_t)expr->value);
				expr = expr->next;
			}
			break;

		case INSN_RESB:
			count = insn->u.expr->value;
			while (count--) {
				vma_output_u8(0xCC);
			}
			break;

		case INSN_RESH:
			count = insn->u.expr->value;
			while (count--) {
				vma_output_u16(0xCCCC);
			}
			break;
			
		case INSN_RESW:
			count = insn->u.expr->value;
			while (count--) {
				vma_output_u32(0xCCCCCCCC);
			}
			break;
	}
}

void vma_assemble(vma_context_t *ctx)
{
	VMA_ASSERT(ctx);

	vma_debug_print("vma_assemble: %p: %p/%p", ctx, ctx->insns_head, ctx->insns_tail);
	vma_pass1(ctx);
}
