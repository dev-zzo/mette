#include "vma.h"
#include "vm_opcodes.h"

#include "vma_insn.ops.tab"

struct vma_insn_node *vma_build_insn(enum vma_insn_type type)
{
	struct vma_insn_node *insn = (struct vma_insn_node *)vma_malloc(sizeof(*insn));

	insn->next = NULL;
	insn->type = type;

	return insn;
}

void vma_output_insn(struct vma_insn_node *node)
{
	uint32_t diff;
	uint32_t count;
	struct vma_expr_node *expr;

	VMA_ASSERT(node);

	if (node->type < INSN_ASM_KEYWORDS) {
		vma_output_u8(vma_insn_to_opcode[node->type]);
	}

	switch (node->type) {
		case INSN_LDC_8_U:
		case INSN_LDC_8_S:
		case INSN_LOCALS:
		case INSN_LDLOC:
		case INSN_STLOC:
			vma_output_u8((uint8_t)node->u.expr->value);
			break;

		case INSN_BR_S:
		case INSN_BR_T:
		case INSN_BR_F:
			diff = node->u.expr->value - node->start_addr;
			if (diff & 0xFFFFFF00) {
				vma_error("branch target out of range");
			}
			vma_output_u8((uint8_t)diff);
			break;

		case INSN_NCALL:
			vma_output_u16(0xBABA); /* TODO... */
			break;
			
		case INSN_LDC_32:
			vma_output_u32(node->u.expr->value);
			break;

		case INSN_BR_L:
		case INSN_CALL:
			diff = node->u.expr->value - node->start_addr;
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

struct vma_unit *vma_build_unit()
{
	struct vma_unit *unit = (struct vma_unit *)vma_malloc(sizeof(*unit));

	unit->tail = unit->head = NULL;

	return unit;
}

struct vma_unit *vma_append_unit(struct vma_unit *unit, struct vma_insn_node *node)
{
	if (unit->tail) {
		node->next = NULL;
		unit->tail->next = node;
		unit->tail = node;
	} else {
		unit->tail = unit->head = node;
	}

	return unit;
}
