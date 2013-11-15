#ifndef __mette_vm_loader_h_included
#define __mette_vm_loader_h_included

#include "vm_internal.h"

typedef struct _vm_section_t {
	struct _vm_section_t *next;
	void *base;
	size_t limit;
} vm_section_t;

/* Load a section from the given fd. */
extern vm_section_t *vm_load_fd(vm_context_t *ctx, int fd);

#endif // __mette_vm_loader_h_included

