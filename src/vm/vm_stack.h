#ifndef __mette_vm_stack_h_included
#define __mette_vm_stack_h_included

#include "vm_sysdeps.h"

typedef struct _vm_stack vm_stack_t;

extern vm_stack_t *vm_stack_create(unsigned max_size);
extern void vm_stack_push(vm_stack_t *stack, vm_operand_t src);
extern void vm_stack_push3(vm_stack_t *stack, const vm_operand_t *src, unsigned count);
extern vm_operand_t vm_stack_pop(vm_stack_t *stack);
extern void vm_stack_pop3(vm_stack_t *stack, vm_operand_t *dst, unsigned count);
extern vm_operand_t *vm_stack_top(vm_stack_t *stack);

#endif // __mette_vm_stack_h_included

