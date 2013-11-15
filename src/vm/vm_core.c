#include "vm_internal.h"
#include "vm_opcodes.h"
#include "vm_opcodes_traits.h"

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

#include "vm_opcodes_traits.tab"

/* Emulate one VM insn. */
void vm_step(vm_context_t *ctx)
{
#include "vm_opcodes_switch.tab"
	vm_operand_t scratchpad[4];
	vm_opcode_t opcode = (vm_opcode_t)vm_fetch8(ctx);
	const vm_opcode_traits_t *traits = &vm_opcode_traits[opcode];
	int sop_count, sop_index;

	sop_count = traits->pop_count;
	sop_index = 0;
	while (sop_count) {
		scratchpad[sop_index] = vm_stack_pop(&ctx->dstack);
		sop_index++;
		sop_count--;
	}
	
	goto *(&&op_invalid + offtab[opcode]);
	
op_invalid:
	vm_panic("vm_step: unknown opcode.");

op_ADD:
	scratchpad[0] = scratchpad[0] + scratchpad[1];
	goto op_push;
op_SUB:
	scratchpad[0] = scratchpad[0] - scratchpad[1];
	goto op_push;
op_UMUL: {
	uint64_t res = (uint64_t)scratchpad[0] * (uint64_t)scratchpad[1];
	scratchpad[0] = res >> 32;
	scratchpad[1] = res & 0xFFFFFFFFULL;
	goto op_push;
}
op_SMUL: {
	int64_t res = ((int64_t)scratchpad[0]) * ((int64_t)scratchpad[1]);
	scratchpad[0] = res >> 32;
	scratchpad[1] = (vm_operand_t)res;
	goto op_push;
}
op_UDIV: {
	vm_uoperand_t quot = (vm_uoperand_t)scratchpad[0] / (vm_uoperand_t)scratchpad[1];
	vm_uoperand_t rem = (vm_uoperand_t)scratchpad[0] % (vm_uoperand_t)scratchpad[1];
	scratchpad[0] = rem;
	scratchpad[1] = quot;
	goto op_push;
}
op_SDIV: {
	vm_soperand_t quot = (vm_soperand_t)scratchpad[0] / (vm_soperand_t)scratchpad[1];
	vm_soperand_t rem = (vm_soperand_t)scratchpad[0] % (vm_soperand_t)scratchpad[1];
	scratchpad[0] = rem;
	scratchpad[1] = quot;
	goto op_push;
}
op_AND:
	scratchpad[0] = scratchpad[0] & scratchpad[1];
	goto op_push;
op_OR:
	scratchpad[0] = scratchpad[0] | scratchpad[1];
	goto op_push;
op_XOR:
	scratchpad[0] = scratchpad[0] ^ scratchpad[1];
	goto op_push;
op_NOT:
	scratchpad[0] = ~scratchpad[0];
	goto op_push;
op_LSL:
	scratchpad[0] = scratchpad[0] << scratchpad[1];
	goto op_push;
op_LSR:
	scratchpad[0] = ((vm_uoperand_t)scratchpad[0]) >> scratchpad[1];
	goto op_push;
op_ASR:
	scratchpad[0] = ((vm_soperand_t)scratchpad[0]) >> scratchpad[1];
op_CC_LT:
	scratchpad[0] = ((vm_soperand_t)scratchpad[0]) < ((vm_soperand_t)scratchpad[1]);
	goto op_push;
op_CC_GT:
	scratchpad[0] = ((vm_soperand_t)scratchpad[0]) > ((vm_soperand_t)scratchpad[1]);
	goto op_push;
op_CC_LE:
	scratchpad[0] = ((vm_soperand_t)scratchpad[0]) <= ((vm_soperand_t)scratchpad[1]);
	goto op_push;
op_CC_GE:
	scratchpad[0] = ((vm_soperand_t)scratchpad[0]) >= ((vm_soperand_t)scratchpad[1]);
	goto op_push;
op_CC_B:
	scratchpad[0] = ((vm_uoperand_t)scratchpad[0]) < ((vm_uoperand_t)scratchpad[1]);
	goto op_push;
op_CC_A:
	scratchpad[0] = ((vm_uoperand_t)scratchpad[0]) > ((vm_uoperand_t)scratchpad[1]);
	goto op_push;
op_CC_BE:
	scratchpad[0] = ((vm_uoperand_t)scratchpad[0]) <= ((vm_uoperand_t)scratchpad[1]);
	goto op_push;
op_CC_AE:
	scratchpad[0] = ((vm_uoperand_t)scratchpad[0]) >= ((vm_uoperand_t)scratchpad[1]);
	goto op_push;
op_CC_EQ:
	scratchpad[0] = scratchpad[0] == scratchpad[1];
	goto op_push;
op_CC_NE:
	scratchpad[0] = scratchpad[0] != scratchpad[1];
	goto op_push;
op_LDC0:
	scratchpad[0] = 0;
	goto op_push;
op_LDC1: 
	scratchpad[0] = 1;
	goto op_push;
op_LDC2: 
	scratchpad[0] = 2;
	goto op_push;
op_LDCU8:
	scratchpad[0] = (vm_uoperand_t)(uint8_t)vm_fetch8(ctx);
	goto op_push;
op_LDCS8:
	scratchpad[0] = (vm_soperand_t)(int8_t)vm_fetch8(ctx);
	goto op_push;
op_LDC32:
	scratchpad[0] = vm_fetch32(ctx);
	goto op_push;
op_LDMU8:
	scratchpad[0] = (vm_uoperand_t)*(uint8_t *)(scratchpad[0]);
	goto op_push;
op_LDMS8:
	scratchpad[0] = (vm_soperand_t)*(int8_t *)(scratchpad[0]);
	goto op_push;
op_LDMU16:
	scratchpad[0] = (vm_uoperand_t)*(uint16_t *)(scratchpad[0]);
	goto op_push;
op_LDMS16:
	scratchpad[0] = (vm_soperand_t)*(int16_t *)(scratchpad[0]);
	goto op_push;
op_LDM32:
	scratchpad[0] = *(vm_operand_t *)(scratchpad[0]);
	goto op_push;
op_STU8:
	*(uint8_t *)(scratchpad[0]) = (uint8_t)scratchpad[1];
	goto op_push;
op_STU16:
	*(uint16_t *)(scratchpad[0]) = (uint16_t)scratchpad[1];
	goto op_push;
op_STM32:
	*(vm_operand_t *)(scratchpad[0]) = scratchpad[1];
	goto op_push;
op_DUP:
	scratchpad[1] = scratchpad[0];
	goto op_push;
op_SWAP: {
	vm_operand_t temp;
	temp = scratchpad[0];
	scratchpad[0] = scratchpad[1];
	scratchpad[1] = temp;
	goto op_push;
}
op_POP:
	goto op_push;
op_BRS: {
	int8_t offset = (int8_t)vm_fetch8(ctx);
	ctx->pc += offset;
	goto op_push;
}
op_BRL: {
	int32_t offset = (int32_t)vm_fetch32(ctx);
	ctx->pc += offset;
	goto op_push;
}
op_BRT: {
	int8_t offset = (int8_t)vm_fetch8(ctx);
	if (scratchpad[0]) {
		ctx->pc += offset;
	}
	goto op_push;
}
op_BRF: {
	int8_t offset = (int8_t)vm_fetch8(ctx);
	if (!scratchpad[0]) {
		ctx->pc += offset;
	}
	goto op_push;
}
op_CALL: {
	int32_t offset = (int32_t)vm_fetch32(ctx);
	vm_stack_push(&ctx->cstack, (vm_operand_t)ctx->pc);
	ctx->pc += offset;
	goto op_push;
}
op_RET: {
	void *target = (void *)vm_stack_pop(&ctx->cstack);
	ctx->pc = target;
	goto op_push;
}
op_NCALL: {
	goto op_push;
}
	
op_push:
	sop_count = traits->push_count;
	sop_index = 0;
	while (sop_count) {
		vm_stack_push(&ctx->dstack, scratchpad[sop_index]);
		sop_index++;
		sop_count--;
	}
	
}

