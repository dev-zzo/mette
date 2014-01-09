#include "vma.h"
#include <stdint.h>

static void vma_resolve_symrefs_expr(struct vma_expr_node *node)
{
	VMA_ASSERT(node);

	switch (node->type) {
		case EXPR_SYMREF:
			vma_resolve_symref(&node->u.symref);
			break;

		case EXPR_OR:
		case EXPR_AND:
		case EXPR_XOR:
		case EXPR_ADD:
		case EXPR_SUB:
		case EXPR_MUL:
		case EXPR_DIV:
			vma_resolve_symrefs_expr(node->u.child[1]);
			/* fall-through */

		case EXPR_NEG:
		case EXPR_NOT:
			vma_resolve_symrefs_expr(node->u.child[0]);
			break;

		default:
			break;
	}
}

static void vma_resolve_symrefs_expr_list(struct vma_expr_list *list)
{
	struct vma_expr_node *node;

	VMA_ASSERT(list);

	node = list->head;
	while (node) {
		vma_resolve_symrefs_expr(node);
		node = node->next;
	}
}

static void vma_resolve_symrefs(struct vma_insn_node *list_head)
{
	struct vma_insn_node *node = list_head;

	while (node) {
		node = node->next;
		switch (node->type) {
			case INSN_LDC_8_U:
			case INSN_LDC_8_S:
			case INSN_LDC_32:
			case INSN_LOCALS:
			case INSN_LDLOC:
			case INSN_STLOC:
				vma_resolve_symrefs_expr(node->u.expr);
				break;
				
			case INSN_DEFB:
			case INSN_RESB:
			case INSN_DEFH:
			case INSN_RESH:
			case INSN_DEFW:
			case INSN_RESW:
				vma_resolve_symrefs_expr_list(node->u.expr_list);
				break;

			case INSN_BR_S:
			case INSN_BR_L:
			case INSN_BR_T:
			case INSN_BR_F:
			case INSN_CALL:
			case INSN_NCALL:
				vma_resolve_symref(&node->u.symref);
				break;
		}
	}
}

/* Walk the list and estimate insn VAs and sizes */
static void vma_estimate_insns(struct vma_insn_node *list_head, vma_vaddr_t start_va)
{
	struct vma_insn_node *node = list_head;
	vma_vaddr_t next_va = start_va;

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
			case INSN_CMP_LE:
			case INSN_CMP_GE:
			case INSN_CMP_B:
			case INSN_CMP_A:
			case INSN_CMP_BE:
			case INSN_CMP_AE:
			case INSN_CMP_EQ:
			case INSN_CMP_NE:
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
				node->length = 1;
				break;

			case INSN_LDC_8_U:
			case INSN_LDC_8_S:
			case INSN_LOCALS:
			case INSN_LDLOC:
			case INSN_STLOC:
			case INSN_BR_S:
			case INSN_BR_T:
			case INSN_BR_F:
				node->length = 1 + 1;
				break;

			case INSN_NCALL:
				node->length = 1 + 2;
				break;
				
			case INSN_LDC_32:
			case INSN_BR_L:
			case INSN_CALL:
				node->length = 1 + 4;
				break;
				
			case INSN_DEFB:
			case INSN_RESB:
				VMA_ASSERT(node->u.expr_list);
				node->length = sizeof(uint8_t) * node->u.expr_list->count;
				break;

			case INSN_DEFH:
			case INSN_RESH:
				VMA_ASSERT(node->u.expr_list);
				node->length = sizeof(uint16_t) * node->u.expr_list->count;
				break;

			case INSN_DEFW:
			case INSN_RESW:
				VMA_ASSERT(node->u.expr_list);
				node->length = sizeof(uint32_t) * node->u.expr_list->count;
				break;

			default:
				vma_abort("%s:%d:internal error: unhandled insn type %d", __FILE__, __LINE__, node->type);
				break;
		}

		next_va += node->length;
		node = node->next;
	}
}

void vma_assemble(struct vma_unit *unit)
{
	VMA_ASSERT(unit);

	vma_resolve_symrefs(unit->head);
	vma_abort_on_errors();
	vma_estimate_insns(unit->head, 0x00400000);
	vma_abort_on_errors();
}
