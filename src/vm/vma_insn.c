#include "vma.h"

struct vma_insn_node *vma_build_insn(enum vma_insn_type type)
{
	struct vma_insn_node *insn = (struct vma_insn_node *)vma_malloc(sizeof(*insn));

	insn->next = NULL;
	insn->type = type;

	return insn;
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
