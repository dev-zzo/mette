#include "vm_internal.h"
#include "vm_opcodes.h"

#define VM_ARCH_MLOAD32(a) (*(vm_operand_t *)(a))
#define VM_ARCH_MSTORE32(a,x) *(vm_operand_t *)(a) = (x)

/* A small helper function to fetch a byte with PC increment. */
static uint8_t vm_fetch8(vm_context_t *ctx)
{
	uint8_t v = *ctx->pc;
	ctx->pc += 1;
	return v;
}

/* A small helper function to fetch a word with PC increment. */
static uint32_t vm_fetch32(vm_context_t *ctx)
{
	vm_operand_t v = VM_ARCH_MLOAD32(ctx->pc);
#ifdef TARGET_IS_BE
	v = __builtin_bswap32(v);
#endif
	ctx->pc += sizeof(vm_operand_t);
	return v;
}

/* Emulate one VM insn. */
void vm_step(vm_context_t *ctx)
{
	vm_opcode_t opcode = (vm_opcode_t)vm_fetch8(ctx);
	
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
		
		case VMOP_AND: {
			vm_operand_t lhs, rhs, result;
			lhs = vm_stack_pop(&ctx->dstack);
			rhs = vm_stack_pop(&ctx->dstack);
			result = lhs & rhs;
			vm_stack_push(&ctx->dstack, result);
			break;
		}
		
		case VMOP_OR: {
			vm_operand_t lhs, rhs, result;
			lhs = vm_stack_pop(&ctx->dstack);
			rhs = vm_stack_pop(&ctx->dstack);
			result = lhs | rhs;
			vm_stack_push(&ctx->dstack, result);
			break;
		}
		
		case VMOP_XOR: {
			vm_operand_t lhs, rhs, result;
			lhs = vm_stack_pop(&ctx->dstack);
			rhs = vm_stack_pop(&ctx->dstack);
			result = lhs ^ rhs;
			vm_stack_push(&ctx->dstack, result);
			break;
		}
		
		case VMOP_NOT: {
			vm_operand_t lhs, result;
			lhs = vm_stack_pop(&ctx->dstack);
			result = ~lhs;
			vm_stack_push(&ctx->dstack, result);
			break;
		}
		
		case VMOP_LD0: {
			vm_stack_push(&ctx->dstack, 0);
			break;
		}
		
		case VMOP_LD1: {
			vm_stack_push(&ctx->dstack, 1);
			break;
		}
		
		case VMOP_LDU8: {
			vm_operand_t value = (uint8_t)vm_fetch8(ctx);
			vm_stack_push(&ctx->dstack, value);
			break;
		}
		
		case VMOP_LDS8: {
			vm_soperand_t value = (int8_t)vm_fetch8(ctx);
			vm_stack_push(&ctx->dstack, (vm_operand_t)value);
			break;
		}
		
		case VMOP_LD32: {
			vm_operand_t value = vm_fetch32(ctx);
			vm_stack_push(&ctx->dstack, value);
			break;
		}
		
		case VMOP_LDMU8: {
			vm_operand_t addr = vm_stack_pop(&ctx->dstack);
			vm_operand_t value = *(uint8_t *)(addr);
			vm_stack_push(&ctx->dstack, value);
			break;
		}
		
		case VMOP_LDMS8: {
			vm_operand_t addr = vm_stack_pop(&ctx->dstack);
			vm_operand_t value = *(int8_t *)(addr);
			vm_stack_push(&ctx->dstack, value);
			break;
		}
		
		case VMOP_LDMU16: {
			vm_operand_t addr = vm_stack_pop(&ctx->dstack);
			vm_operand_t value = *(uint16_t *)(addr);
			vm_stack_push(&ctx->dstack, value);
			break;
		}
		
		case VMOP_LDMS16: {
			vm_operand_t addr = vm_stack_pop(&ctx->dstack);
			vm_operand_t value = *(int16_t *)(addr);
			vm_stack_push(&ctx->dstack, value);
			break;
		}
		
		case VMOP_LDM32: {
			vm_operand_t addr = vm_stack_pop(&ctx->dstack);
			vm_operand_t value = *(vm_operand_t *)(addr);
			vm_stack_push(&ctx->dstack, value);
			break;
		}
		
		case VMOP_STMU8: {
			vm_operand_t addr = vm_stack_pop(&ctx->dstack);
			vm_operand_t value = vm_stack_pop(&ctx->dstack);
			*(uint8_t *)addr = (uint8_t)value;
			break;
		}
		
		case VMOP_STMS8: {
			vm_operand_t addr = vm_stack_pop(&ctx->dstack);
			vm_operand_t value = vm_stack_pop(&ctx->dstack);
			*(int8_t *)addr = (int8_t)value;
			break;
		}
		
		case VMOP_STMU16: {
			vm_operand_t addr = vm_stack_pop(&ctx->dstack);
			vm_operand_t value = vm_stack_pop(&ctx->dstack);
			*(uint16_t *)addr = (uint16_t)value;
			break;
		}
		
		case VMOP_STMS16: {
			vm_operand_t addr = vm_stack_pop(&ctx->dstack);
			vm_operand_t value = vm_stack_pop(&ctx->dstack);
			*(int16_t *)addr = (int16_t)value;
			break;
		}
		
		case VMOP_STM32: {
			vm_operand_t addr = vm_stack_pop(&ctx->dstack);
			vm_operand_t value = vm_stack_pop(&ctx->dstack);
			*(vm_operand_t *)addr = value;
			break;
		}
		
		case VMOP_DUP: {
			vm_operand_t value = vm_stack_pop(&ctx->dstack);
			vm_stack_push(&ctx->dstack, value);
			vm_stack_push(&ctx->dstack, value);
			break;
		}
		
		case VMOP_SWAP: {
			vm_operand_t value1 = vm_stack_pop(&ctx->dstack);
			vm_operand_t value2 = vm_stack_pop(&ctx->dstack);
			vm_stack_push(&ctx->dstack, value1);
			vm_stack_push(&ctx->dstack, value2);
			break;
		}
		
		case VMOP_POP: {
			vm_stack_pop(&ctx->dstack);
			break;
		}
		
		case VMOP_BRS: {
			int8_t offset = (int8_t)vm_fetch8(ctx);
			ctx->pc += offset;
			break;
		}
		
		case VMOP_BRL: {
			int32_t offset = (int32_t)vm_fetch32(ctx);
			ctx->pc += offset;
			break;
		}
		
		case VMOP_BRC: {
			int8_t offset = (int8_t)vm_fetch8(ctx);
			vm_operand_t cond = vm_stack_pop(&ctx->dstack);
			if (cond) {
				ctx->pc += offset;
			}
			break;
		}
		
		case VMOP_BRNC: {
			int8_t offset = (int8_t)vm_fetch8(ctx);
			vm_operand_t cond = vm_stack_pop(&ctx->dstack);
			if (!cond) {
				ctx->pc += offset;
			}
			break;
		}
		
		case VMOP_CALL: {
			int32_t offset = (int32_t)vm_fetch32(ctx);
			vm_stack_push(&ctx->cstack, (vm_operand_t)ctx->pc);
			ctx->pc += offset;
			break;
		}
		
		case VMOP_RET: {
			void *target = (void *)vm_stack_pop(&ctx->cstack);
			ctx->pc = target;
		}
		
		default:
			vm_panic("vm_step: unknown opcode.");
	}
}

