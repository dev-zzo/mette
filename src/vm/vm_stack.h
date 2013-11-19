#ifndef __mette_vm_stack_h_included
#define __mette_vm_stack_h_included

#include "vm_sysdeps.h"

#define VM_CHUNK_ENTRIES (( 4096 / sizeof(vm_operand_t) ) - 1)

typedef struct _vm_chunk {
	struct _vm_chunk *next;
	vm_operand_t entries[VM_CHUNK_ENTRIES];
} vm_chunk_t;

/* VM stack composite */

typedef struct _vm_stack {
	vm_chunk_t *head;
	vm_chunk_t *top_chunk;
	int top_index; /* signed. */
} vm_stack_t;

/* Push the entry onto the stack */
extern void vm_stack_push(vm_stack_t *stack, vm_operand_t value);
/* Pop an entry from the stack */
extern vm_operand_t vm_stack_pop(vm_stack_t *stack);
/* Pop n entries from the stack */
extern void vm_stack_popn(vm_stack_t *stack, size_t n, vm_operand_t *dst);
/* Find a stack entry with the given index from the top */
extern vm_operand_t *vm_stack_ptr(vm_stack_t *stack, int index);

#endif // __mette_vm_stack_h_included

