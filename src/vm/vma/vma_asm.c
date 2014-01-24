#include "vma.h"
#include <stdint.h>

/*******************************************************************************
 * Symbols
 */

void vma_symtab_init(vma_symtab_t *symtab)
{
	symtab->tail = symtab->head = NULL;
	symtab->count = 0;
}

vma_symbol_t *vma_symtab_define(vma_symtab_t *symtab, const char *name, int unique)
{
	vma_symbol_t *sym = vma_symtab_lookup(symtab, name);
	if (sym) {
		if (unique) {
			vma_error("symbol '%s' already defined", name);
			return NULL;
		}
		return sym;
	}

	sym = (vma_symbol_t *)vma_malloc(sizeof(*sym));

	sym->next = NULL;
	sym->name = name;
	sym->u.id = symtab->count++;
	if (symtab->tail) {
		symtab->tail->next = sym;
	} else {
		symtab->head = sym;
	}
	symtab->tail = sym;

	vma_debug_print("symtab %p: new symbol: `%s' (%d/%p)", symtab, name, sym->u.id, sym);

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

	sym = vma_symtab_lookup(symtab, ref->u.name);
	if (!sym) {
		vma_error("unresolved symbol: `%s'", ref->u.name);
		return 0;		
	}
	vma_debug_print("symref %p: symbol: `%s' -> %p (insn %p)", ref, ref->u.name, sym, sym->u.location);

	ref->u.sym = sym;
	return 1;
}

/*******************************************************************************
 * Expressions
 */

vma_expr_t *vma_expr_build_constant(int value)
{
	vma_expr_t *node = (vma_expr_t *)vma_malloc(sizeof(*node));

	node->next = NULL;
	node->type = EXPR_CONSTANT;
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

uint32_t vma_expr_evaluate(vma_expr_t *expr, vma_context_t *ctx)
{
	uint32_t lhs, rhs;

	VMA_ASSERT(expr);

	vma_debug_print("evaluating expr: %p", expr);
	switch (expr->type) {
		case EXPR_CONSTANT:
			break;

		case EXPR_SYMREF:
			if (vma_symref_resolve(&expr->u.symref, &ctx->labels)) {
				expr->value = expr->u.symref.u.sym->u.location->start_addr;
			} else {
				expr->value = 0;
			}
			break;

		case EXPR_OR:
			lhs = vma_expr_evaluate(expr->u.child[0], ctx);
			rhs = vma_expr_evaluate(expr->u.child[1], ctx);
			expr->value = lhs | rhs;
			break;

		case EXPR_AND:
			lhs = vma_expr_evaluate(expr->u.child[0], ctx);
			rhs = vma_expr_evaluate(expr->u.child[1], ctx);
			expr->value = lhs & rhs;
			break;

		case EXPR_XOR:
			lhs = vma_expr_evaluate(expr->u.child[0], ctx);
			rhs = vma_expr_evaluate(expr->u.child[1], ctx);
			expr->value = lhs ^ rhs;
			break;

		case EXPR_ADD:
			lhs = vma_expr_evaluate(expr->u.child[0], ctx);
			rhs = vma_expr_evaluate(expr->u.child[1], ctx);
			expr->value = lhs + rhs;
			break;

		case EXPR_SUB:
			lhs = vma_expr_evaluate(expr->u.child[0], ctx);
			rhs = vma_expr_evaluate(expr->u.child[1], ctx);
			expr->value = lhs - rhs;
			break;

		case EXPR_MUL:
			lhs = vma_expr_evaluate(expr->u.child[0], ctx);
			rhs = vma_expr_evaluate(expr->u.child[1], ctx);
			expr->value = lhs * rhs;
			break;

		case EXPR_DIV:
			lhs = vma_expr_evaluate(expr->u.child[0], ctx);
			rhs = vma_expr_evaluate(expr->u.child[1], ctx);
			if (rhs == 0) {
				/* TODO: location tracking. */
				vma_error("divisor evaluates to zero");
				expr->value = 1;
			} else {
				expr->value = lhs / rhs;
			}
			break;

		case EXPR_NEG:
			rhs = vma_expr_evaluate(expr->u.child[0], ctx);
			expr->value = -rhs;
			break;

		case EXPR_NOT:
			rhs = vma_expr_evaluate(expr->u.child[0], ctx);
			expr->value = ~rhs;
			break;

		default:
			vma_abort("%s:%d:internal error: unhandled expr type %d", __FILE__, __LINE__, expr->type);
			return 0;
	}
	return expr->value;
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

void vma_expr_list_evaluate(vma_expr_list_t *list, vma_context_t *ctx)
{
	vma_expr_t *expr;

	VMA_ASSERT(list);

	expr = list->head;
	while (expr) {
		vma_expr_evaluate(expr, ctx);
		expr = expr->next;
	}
}

/*******************************************************************************
 * Instructions
 */

vma_insn_t *vma_insn_build(vma_insn_type_t type)
{
	vma_insn_t *insn = (vma_insn_t *)vma_malloc(sizeof(*insn));

	insn->next = NULL;
	insn->type = type;

	return insn;
}

void vma_insn_evaluate(vma_insn_t *insn, vma_context_t *ctx)
{
	vma_debug_print("evaluating insn: %p", insn);
	switch (insn->type) {
		case INSN_LDC_8_U:
		case INSN_LDC_8_S:
		case INSN_LDC_32:
		case INSN_LOCALS:
		case INSN_LDLOC:
		case INSN_STLOC:
		case INSN_RESB:
		case INSN_RESH:
		case INSN_RESW:
			vma_expr_evaluate(insn->u.expr, ctx);
			break;
			
		case INSN_DEFB:
		case INSN_DEFH:
		case INSN_DEFW:
			vma_expr_list_evaluate(insn->u.expr_list, ctx);
			break;

		case INSN_LEA:
		case INSN_BR:
		case INSN_BR_T:
		case INSN_BR_F:
		case INSN_CALL:
			vma_symref_resolve(&insn->u.symref, &ctx->labels);
			break;

		case INSN_NCALL:
			//vma_symref_resolve(&insn->u.symref, &ctx->ncalls);
			break;
	}
}

#include "vm_opcodes.codes.tab"

void vma_insn_emit(vma_insn_t *node)
{
	uint32_t diff;
	uint32_t count;
	vma_expr_t *expr;

	VMA_ASSERT(node);

	if (node->type < INSN_ASM_KEYWORDS) {
		vma_output_u8(vm_insn_to_opcode[node->type]);
	}

	switch (node->type) {
		case INSN_LDC_8_U:
		case INSN_LDC_8_S:
		case INSN_LOCALS:
		case INSN_LDLOC:
		case INSN_STLOC:
			vma_output_u8((uint8_t)node->u.expr->value);
			break;

		case INSN_BR:
		case INSN_BR_T:
		case INSN_BR_F:
			/* Relative to insn end. */
			VMA_ASSERT(node->u.symref.u.sym);
			diff = node->u.symref.u.sym->u.location->start_addr - (node->start_addr + 2); /* hard-coded :( */
			if (diff & 0xFFFFFF00) {
				vma_error("branch target out of range");
			}
			vma_output_u8((uint8_t)diff);
			break;

		case INSN_NCALL:
			VMA_ASSERT(node->u.symref.u.sym);
			vma_output_u16(node->u.symref.u.sym->u.id);
			break;
			
		case INSN_LDC_32:
			vma_output_u32(node->u.expr->value);
			break;

		case INSN_LEA:
		case INSN_CALL:
			/* Relative to insn end. */
			VMA_ASSERT(node->u.symref.u.sym);
			diff = node->u.symref.u.sym->u.location->start_addr - (node->start_addr + 5); /* hard-coded :( */
			vma_output_u32(diff);
			break;
			
		case INSN_DEFB:
			expr = node->u.expr_list->head;
			while (expr) {
				vma_output_u8((uint8_t)expr->value);
				expr = expr->next;
			}
			break;

		case INSN_DEFH:
			expr = node->u.expr_list->head;
			while (expr) {
				vma_output_u16((uint16_t)expr->value);
				expr = expr->next;
			}
			break;

		case INSN_DEFW:
			expr = node->u.expr_list->head;
			while (expr) {
				vma_output_u32((uint32_t)expr->value);
				expr = expr->next;
			}
			break;

		case INSN_DEFS: {
				const char *text = node->u.text.buffer;
				unsigned count = node->u.text.length;
				while (count) {
					vma_output_u8(*text);
					++text;
					--count;
				}
			}
			break;

		case INSN_RESB:
			count = node->u.expr->value;
			while (count--) {
				vma_output_u8(0xCC);
			}
			break;

		case INSN_RESH:
			count = node->u.expr->value;
			while (count--) {
				vma_output_u16(0xCCCC);
			}
			break;
			
		case INSN_RESW:
			count = node->u.expr->value;
			while (count--) {
				vma_output_u32(0xCCCCCCCC);
			}
			break;
	}
}

/*******************************************************************************
 * Assembly
 */

static void vma_insns_evaluate(vma_context_t *ctx)
{
	vma_insn_t *node = ctx->insns_head;

	vma_debug_print("evaluating insns...");
	while (node) {
		vma_insn_evaluate(node, ctx);
		node = node->next;
	}
}

/* Walk the list and estimate insn VAs and sizes */
static void vma_insns_allocate(vma_context_t *ctx)
{
	vma_insn_t *node = ctx->insns_head;
	vma_vaddr_t next_va = ctx->start_va;
	vma_vaddr_t bss_va = ctx->start_va;

	vma_debug_print("evaluating insn lengths...");
	while (node) {
		node->start_addr = next_va;
		switch (node->type) {
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
			case INSN_LDM_8_U:
			case INSN_LDM_8_S:
			case INSN_LDM_16_U:
			case INSN_LDM_16_S:
			case INSN_LDM_32:
			case INSN_STM_8:
			case INSN_STM_16:
			case INSN_STM_32:
			case INSN_DUP:
			case INSN_SWAP:
			case INSN_POP:
			case INSN_RET:
			case INSN_ICALL:
			case INSN_IJMP:
				next_va += 1;
				bss_va = next_va;
				break;

			case INSN_LDC_8_U:
			case INSN_LDC_8_S:
			case INSN_LOCALS:
			case INSN_LDLOC:
			case INSN_STLOC:
			case INSN_BR:
			case INSN_BR_T:
			case INSN_BR_F:
				next_va += 1 + 1;
				bss_va = next_va;
				break;

			case INSN_NCALL:
				next_va += 1 + 2;
				bss_va = next_va;
				break;
				
			case INSN_LDC_32:
			case INSN_LEA:
			case INSN_CALL:
				next_va += 1 + 4;
				bss_va = next_va;
				break;
				
			case INSN_DEFS:
				VMA_ASSERT(node->u.text.buffer);
				next_va += node->u.text.length;
				bss_va = next_va;
				break;

			case INSN_DEFB:
				VMA_ASSERT(node->u.expr_list);
				next_va += sizeof(uint8_t) * node->u.expr_list->count;
				bss_va = next_va;
				break;

			case INSN_DEFH:
				VMA_ASSERT(node->u.expr_list);
				node->start_addr = VMA_ALIGN(node->start_addr, 2);
				next_va = VMA_ALIGN(next_va, 2);
				next_va += sizeof(uint16_t) * node->u.expr_list->count;
				bss_va = next_va;
				break;

			case INSN_DEFW:
				VMA_ASSERT(node->u.expr_list);
				node->start_addr = VMA_ALIGN(node->start_addr, 4);
				next_va = VMA_ALIGN(next_va, 4);
				next_va += sizeof(uint32_t) * node->u.expr_list->count;
				bss_va = next_va;
				break;

			case INSN_RESB:
				VMA_ASSERT(node->u.expr);
				next_va += sizeof(uint8_t) * node->u.expr->value;
				break;

			case INSN_RESH:
				VMA_ASSERT(node->u.expr);
				node->start_addr = VMA_ALIGN(node->start_addr, 2);
				next_va = VMA_ALIGN(next_va, 2);
				next_va += sizeof(uint16_t) * node->u.expr->value;
				break;

			case INSN_RESW:
				VMA_ASSERT(node->u.expr);
				node->start_addr = VMA_ALIGN(node->start_addr, 4);
				next_va = VMA_ALIGN(next_va, 4);
				next_va += sizeof(uint32_t) * node->u.expr->value;
				break;

			default:
				vma_abort("%s:%d:internal error: unhandled insn type %d", __FILE__, __LINE__, node->type);
				break;
		}

		vma_debug_print("alloc: %p (%02X): at %08X", node, node->type, node->start_addr);

		node = node->next;
	}

	ctx->end_va = next_va;
	ctx->bss_va = bss_va;
}

void vma_assemble(vma_context_t *ctx)
{
	VMA_ASSERT(ctx);

	vma_debug_print("vma_assemble: %p: %p/%p", ctx, ctx->insns_head, ctx->insns_tail);
	vma_insns_evaluate(ctx);
	vma_abort_on_errors();
	vma_insns_allocate(ctx);
	vma_abort_on_errors();

	vma_symtab_dump(&ctx->labels, 1);
}
