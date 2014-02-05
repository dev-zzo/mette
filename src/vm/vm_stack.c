#include "vm_stack.h"
#include "vm_sysdeps.h"
#include "vm_misc.h"
#include "vm_internal.h"

//#define DEBUG_PRINTS
#include "rtl_debug.h"

typedef struct _vm_stack {
	vm_operand_t *bottom;
	vm_operand_t *limit;
	vm_operand_t *top;
} vm_stack_t;

vm_stack_t *vm_stack_create(unsigned max_size)
{
	vm_stack_t *stack;
	vm_operand_t *stack_mem;

	stack = vm_alloc(sizeof(*stack));
	if (!stack) {
		vm_panic("vm_stack_create: vm_alloc failed.");
	}

	stack_mem = (vm_operand_t *)vm_mmap(
		NULL, max_size, 
		PROT_READ|PROT_WRITE, 
		MAP_PRIVATE|MAP_ANONYMOUS|MAP_GROWSDOWN,
		-1, 0);
	if (stack_mem == MAP_FAILED) {
		vm_panic("vm_stack_create: vm_mmap failed.");
	}

	stack->bottom = stack_mem + (max_size / sizeof(vm_operand_t));
	stack->top = stack->bottom;
	stack->limit = stack_mem;
	DBGPRINT("vm_stack_create: allocated mem at %08x ~ %08x\n", stack->limit, stack->bottom);

	return stack;
}

void vm_stack_push(vm_stack_t *stack, vm_operand_t src)
{
	vm_stack_push3(stack, &src, 1);
}

void vm_stack_push3(vm_stack_t *stack, const vm_operand_t *src, unsigned count)
{
	vm_operand_t *top = stack->top - count;
	int offset = 0;

	if (top < stack->limit) {
		vm_panic("vm_stack_push: stack overflow");
	}

	while (offset < count) {
		top[offset] = src[offset];
		DBGPRINT("vm_stack_push: %08x\n", top[offset]);
		++offset;
	}

	stack->top = top;
}

void vm_stack_pop3(vm_stack_t *stack, vm_operand_t *dst, unsigned count)
{
	vm_operand_t *top = stack->top;
	int offset = 0;

	if (top + count > stack->bottom) {
		vm_panic("vm_stack_pop: stack underflow");
	}

	while (offset < count) {
		dst[offset] = top[offset];
		DBGPRINT("vm_stack_pop: %08x\n", top[offset]);
		++offset;
	}

	stack->top = top + count;
}
