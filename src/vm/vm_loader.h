#ifndef __mette_vm_loader_h_included
#define __mette_vm_loader_h_included

typedef struct _vm_module_t {
	struct _vm_module_t *next;
	void *base;
	void *entry;
	void **ncalls_table;
} vm_module_t;

/* Load a section from the given fd. */
extern vm_module_t *vm_load_fd(int fd);

#endif // __mette_vm_loader_h_included

