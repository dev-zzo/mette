#include "vm_asm.h"

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

	sym = (struct vma_symbol *)malloc(sizeof(*sym));

	sym->next = vma_symtab;
	sym->name = name;
	sym->location = NULL;

	return sym;
}

void vma_init_symref(struct vma_symref *ref, const char *name)
{
	ref->u.name = name;
	ref->resolved = 0;
	ref->ncall = 0;
}
