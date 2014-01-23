#ifndef __mette_vm_internal_h_included
#define __mette_vm_internal_h_included

#include "vm_stack.h"
#include "vm_loader.h"

/* Keep track of locals */
typedef struct _vm_locals {
	unsigned count; /* How many of them (for boundary checks) */
	vm_operand_t data[1]; /* Can be more. */
} vm_locals_t;

/* VM context -- where all the state is. */
typedef struct _vm_context {
	vm_stack_t dstack; /* Stack for data ops */
	vm_stack_t cstack; /* Stack for control flow ops */
	
	uint8_t *pc;
	vm_locals_t *locals;
	vm_module_t *module; /* Current module */
} vm_context_t;

/* Core VM functions */

extern vm_context_t *vm_context_create(vm_module_t *module);

extern void vm_step(vm_context_t *ctx);

/* Miscellaneous VM functions */

extern void vm_verify_ptr(const void * const ptr);

extern void __attribute__((noreturn)) vm_panic(const char *format, ...);

#endif // __mette_vm_internal_h_included

