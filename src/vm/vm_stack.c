#include "vm_internal.h"

void vm_stack_push(vm_stack_t *stack, vm_operand_t value)
{
	int next_index = stack->top_index + 1;
	
	if (next_index < 0) {
		vm_panic("vm_stack_push: stack underflow detected.");
	}
	
	if (next_offset >= VM_CHUNK_ENTRIES) {
		vm_chunk_t *live = (vm_chunk_t *)vm_alloc(sizeof(vm_chunk_t));
		if (!live) {
			vm_panic("vm_stack_push: vm_alloc failed.");
		}
		
		live->next = stack->top_chunk;
		stack->top_chunk = live;
		next_offset = 0;
	}
	
	stack->entries[next_index] = value;
	stack->top_index = next_index;
}

vm_operand_t vm_stack_pop(vm_stack_t *stack)
{
	vm_operand_t value;
	
	if (stack->top_index < 0 || !stack->top_chunk) {
		vm_panic("vm_stack_pop: stack underflow detected.");
	}
	
	value = stack->top_chunk[stack->top_index];
	
	if (stack->top_offset == 0) {
		vm_chunk_t *dead = stack->top_chunk;
		stack->top_chunk = dead->next;
		stack->top_index = VM_CHUNK_ENTRIES - 1;
		vm_free(dead);
	} else {
		stack->top_index -= 1;
	}
	
	return value;
}

