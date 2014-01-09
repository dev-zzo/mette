#include "vma.h"

static struct vma_symbol *vma_symtab;
static struct vma_symbol *vma_ncall_symtab;

static uint32_t vma_hash_name(const char *name)
{
	uint32_t hash = 0;
	const char *ptr = name;

	while (*ptr) {
		hash = hash * 7 + *ptr;
		ptr++;
	}

	return hash;
}

static struct vma_symbol *vma_lookup_symbol_internal(const char *name, struct vma_symbol *list)
{
	struct vma_symbol *curr = list;
	while (curr) {
		if (!strcmp(name, curr->name)) {
			break;
		}
		curr = curr->next;
	}
	return curr;
}

struct vma_symbol *vma_lookup_symbol(const char *name)
{
	return vma_lookup_symbol_internal(name, vma_symtab);
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
	sym->u.location = NULL;
	vma_symtab = sym;

	vma_debug_print("new symbol defined: '%s'", name);
	return sym;
}

struct vma_symbol *vma_lookup_ncall_symbol(const char *name)
{
	return vma_lookup_symbol_internal(name, vma_ncall_symtab);
}

struct vma_symbol *vma_define_ncall_symbol(const char *name)
{
	struct vma_symbol *sym = vma_lookup_ncall_symbol(name);
	if (sym) {
		return sym;
	}

	sym = (struct vma_symbol *)vma_malloc(sizeof(*sym));

	sym->next = vma_ncall_symtab;
	sym->name = name;
	sym->u.hash = vma_hash_name(name);
	vma_ncall_symtab = sym;

	vma_debug_print("new ncall symbol defined: '%s' -> %08X", name, sym->u.hash);
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

	if (ref->ncall == 0) {
		sym = vma_lookup_symbol(ref->u.name);
		if (!sym) {
			vma_error("unresolved symbol: `%s'", ref->u.name);
			return 0;
		}
	} else {
		sym = vma_lookup_ncall_symbol(ref->u.name);
		if (!sym) {
			sym = vma_define_ncall_symbol(ref->u.name);
		}
	}

	ref->u.sym = sym;
	ref->resolved = 1;
	return 1;
}
