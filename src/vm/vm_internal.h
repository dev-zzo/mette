#ifndef __mette_vm_internal_h_included
#define __mette_vm_internal_h_included

#include <stdint.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

/* Integration defines... */

#include <malloc.h>

#define vm_alloc malloc
#define vm_free free
#define vm_read read

typedef  intptr_t vm_soperand_t;
typedef uintptr_t vm_uoperand_t;
typedef vm_uoperand_t vm_operand_t;

/* VM memory chunks, used for stack management. */

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

