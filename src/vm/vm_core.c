#include "vm_internal.h"
#include "vm_opcodes.h"

void vm_step(vm_content_t *ctx)
{
	vm_opcode_t opcode = (vm_opcode_t)ctx->pc[0];
	
	switch(opcode) {
	case VMOP_ADD: {
		vm_operand_t lhs, rhs, result;
		lhs = vm_stack_pop(&ctx->dstack);
		rhs = vm_stack_pop(&ctx->dstack);
		result = lhs + rhs;
		vm_stack_push(&ctx->dstack, result);
		break;
		}
		
	case VMOP_SUB: {
		vm_operand_t lhs, rhs, result;
		lhs = vm_stack_pop(&ctx->dstack);
		rhs = vm_stack_pop(&ctx->dstack);
		result = lhs - rhs;
		vm_stack_push(&ctx->dstack, result);
		break;
		}
		
	case VMOP_LD0:
		vm_stack_push(&ctx->dstack, 0);
		break;
		
	case VMOP_LD1:
		vm_stack_push(&ctx->dstack, 1);
		break;
		
	default:
		vm_panic("vm_step: unknown opcode.");
	}
}

