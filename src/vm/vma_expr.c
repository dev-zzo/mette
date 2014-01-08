#include "vma.h"

struct vma_expr_node *vma_build_constant_expr(int value)
{
	struct vma_expr_node *node = (struct vma_expr_node *)vma_malloc(sizeof(*node));

	node->next = NULL;
	node->type = EXPR_CONSTANT;
	node->u.const_int = value;

	return node;
}

struct vma_expr_node *vma_build_symref_expr(const char *name)
{
	struct vma_expr_node *node = (struct vma_expr_node *)malloc(sizeof(*node));

	node->next = NULL;
	node->type = EXPR_SYMREF;
	vma_init_symref(&node->u.symref, name);

	return node;
}

struct vma_expr_node *vma_build_parent_expr
(
	enum vma_expr_type type,
	struct vma_expr_node *a,
	struct vma_expr_node *b
)
{
	struct vma_expr_node *node = (struct vma_expr_node *)malloc(sizeof(*node));

	node->next = NULL;
	node->type = type;
	node->u.child[0] = a;
	node->u.child[1] = b;

	return node;
}

struct vma_expr_list *vma_create_expr_list()
{
	struct vma_expr_list *list = (struct vma_expr_list *)vma_malloc(sizeof(*list));
	list->tail = list->head = NULL;
	return list;
}

struct vma_expr_list *vma_append_expr_list(struct vma_expr_list *list, struct vma_expr_node *node)
{
	if (list->tail) {
		list->tail->next = node;
		node->next = NULL;
		list->tail = node;
	} else {
		list->tail = list->head = node;
	}
	return list;
}
