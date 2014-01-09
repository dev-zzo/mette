#include "vma.h"

static struct vma_symbol *vma_symtab;

struct vma_symbol *vma_lookup_symbol(const char *name)
{
	struct vma_symbol *curr = vma_symtab;
	while (curr) {
		if (!strcmp(name, curr->name)) {
			break;
		}
		curr = curr->next;
	}
	return curr;
}

struct vma_symbol *vma_define_symbol(const char *name)
{
	struct vma_symbol *sym = vma_lookup_symbol(name);
	if (sym) {
		vma_error("symbol '%s' already defined", name);
		return NULL;
	}

	sym = (struct vma_symbol *)vma_malloc(sizeof(*sym));

	sym->next = vma_symtab;
	sym->name = name;
	sym->location = NULL;
	vma_symtab = sym;

	vma_debug_print("new symbol defined: '%s'", name);
	return sym;
}

void vma_init_symref(struct vma_symref *ref, const char *name)
{
	ref->u.name = name;
	ref->resolved = 0;
	ref->ncall = 0;
}

int vma_resolve_symref(struct vma_symref *ref)
{
	struct vma_symbol *sym;

	VMA_ASSERT(ref->resolved == 0);
	VMA_ASSERT(ref->u.name);

	sym = vma_lookup_symbol(ref->u.name);
	if (!sym) {
		vma_error("unresolved symbol: `%s'", ref->u.name);
		return 0;
	}

	ref->u.sym = sym;
	ref->resolved = 1;
	return 1;
}