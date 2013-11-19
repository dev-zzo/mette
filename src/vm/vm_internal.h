#ifndef __mette_vm_internal_h_included
#define __mette_vm_internal_h_included

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#include "vm_stack.h"
#include "vm_loader.h"

/* VM context -- where all the state is. */

typedef struct _vm_context {
	vm_stack_t cstack; /* Stack for control flow ops */
	vm_stack_t dstack; /* Stack for data ops */
	
	uint8_t *pc;
} vm_context_t;

/* Miscellaneous VM functions */

extern void vm_verify_ptr(const void * const ptr);

extern void __attribute__((noreturn)) vm_panic(const char *format, ...);

#endif // __mette_vm_internal_h_included

