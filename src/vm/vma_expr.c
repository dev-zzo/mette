#include "vma.h"

struct vma_expr_node *vma_build_constant_expr(int value)
{
	struct vma_expr_node *node = (struct vma_expr_node *)vma_malloc(sizeof(*node));

	node->next = NULL;
	node->type = EXPR_CONSTANT;
	node->value = value;

	return node;
}

struct vma_expr_node *vma_build_symref_expr(const char *name)
{
	struct vma_expr_node *node = (struct vma_expr_node *)vma_malloc(sizeof(*node));

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
	struct vma_expr_node *node = (struct vma_expr_node *)vma_malloc(sizeof(*node));

	node->next = NULL;
	node->type = type;
	node->u.child[0] = a;
	node->u.child[1] = b;

	return node;
}

uint32_t vma_evaluate_expr(struct vma_expr_node *expr)
{
	uint32_t lhs, rhs;

	VMA_ASSERT(expr);

	switch (expr->type) {
		case EXPR_CONSTANT:
			break;

		case EXPR_SYMREF:
			VMA_ASSERT(expr->u.symref.ncall == 0);
			VMA_ASSERT(expr->u.symref.u.sym);
			VMA_ASSERT(expr->u.symref.u.sym->location);
			expr->value = expr->u.symref.u.sym->location->start_addr;
			break;

		case EXPR_OR:
			lhs = vma_evaluate_expr(expr->u.child[0]);
			rhs = vma_evaluate_expr(expr->u.child[1]);
			expr->value = lhs | rhs;
			break;

		case EXPR_AND:
			lhs = vma_evaluate_expr(expr->u.child[0]);
			rhs = vma_evaluate_expr(expr->u.child[1]);
			expr->value = lhs & rhs;
			break;

		case EXPR_XOR:
			lhs = vma_evaluate_expr(expr->u.child[0]);
			rhs = vma_evaluate_expr(expr->u.child[1]);
			expr->value = lhs ^ rhs;
			break;

		case EXPR_ADD:
			lhs = vma_evaluate_expr(expr->u.child[0]);
			rhs = vma_evaluate_expr(expr->u.child[1]);
			expr->value = lhs + rhs;
			break;

		case EXPR_SUB:
			lhs = vma_evaluate_expr(expr->u.child[0]);
			rhs = vma_evaluate_expr(expr->u.child[1]);
			expr->value = lhs - rhs;
			break;

		case EXPR_MUL:
			lhs = vma_evaluate_expr(expr->u.child[0]);
			rhs = vma_evaluate_expr(expr->u.child[1]);
			expr->value = lhs * rhs;
			break;

		case EXPR_DIV:
			lhs = vma_evaluate_expr(expr->u.child[0]);
			rhs = vma_evaluate_expr(expr->u.child[1]);
			if (rhs == 0) {
				/* TODO: location tracking. */
				vma_error("divisor evaluates to zero");
				expr->value = 1;
			} else {
				expr->value = lhs / rhs;
			}
			break;

		case EXPR_NEG:
			rhs = vma_evaluate_expr(expr->u.child[0]);
			expr->value = -rhs;
			break;

		case EXPR_NOT:
			rhs = vma_evaluate_expr(expr->u.child[0]);
			expr->value = ~rhs;
			break;

		default:
			vma_abort("%s:%d:internal error: unhandled expr type %d", __FILE__, __LINE__, expr->type);
			return 0;
	}
	return expr->value;
}


struct vma_expr_list *vma_create_expr_list(void)
{
	struct vma_expr_list *list = (struct vma_expr_list *)vma_malloc(sizeof(*list));
	list->tail = list->head = NULL;
	list->count = 0;
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
	list->count++;
	return list;
}

