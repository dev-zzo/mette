#include "vm_internal.h"
#include "vm_opcodes.h"
#include "vm_opcodes_traits.h"
#include "vm_thunks.h"

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
#ifdef TARGET_IS_BE
	vm_operand_t v = __builtin_bswap32(*(vm_operand_t *)(ctx->pc));
#else
	vm_operand_t v = *(vm_operand_t *)(ctx->pc);
#endif
	ctx->pc += sizeof(vm_operand_t);
	return v;
}

#include "vm_opcodes_traits.tab"

/* Emulate one VM insn. */
void vm_step(vm_context_t *ctx)
{
#include "vm_opcodes_switch.tab"
	vm_operand_t spad[VM_MAX_ARGS];
	vm_opcode_t opcode = (vm_opcode_t)vm_fetch8(ctx);
	const vm_opcode_traits_t *traits = &vm_opcode_traits[opcode];
	int sop_count, sop_index;

	vm_stack_popn(&ctx->dstack, traits->pop_count, spad);
	
	goto *(&&op_invalid + offtab[opcode]);
	
op_invalid:
	vm_panic("vm_step: unknown opcode.");

op_ADD:
	spad[0] = spad[0] + spad[1];
	goto op_push;
op_SUB:
	spad[0] = spad[0] - spad[1];
	goto op_push;
op_UMUL: {
	uint64_t res = (uint64_t)spad[0] * (uint64_t)spad[1];
	spad[0] = res >> 32;
	spad[1] = res & 0xFFFFFFFFULL;
	goto op_push;
}
op_SMUL: {
	int64_t res = ((int64_t)spad[0]) * ((int64_t)spad[1]);
	spad[0] = res >> 32;
	spad[1] = (vm_operand_t)res;
	goto op_push;
}
op_UDIV: {
	vm_uoperand_t quot = (vm_uoperand_t)spad[0] / (vm_uoperand_t)spad[1];
	vm_uoperand_t rem = (vm_uoperand_t)spad[0] % (vm_uoperand_t)spad[1];
	spad[0] = rem;
	spad[1] = quot;
	goto op_push;
}
op_SDIV: {
	vm_soperand_t quot = (vm_soperand_t)spad[0] / (vm_soperand_t)spad[1];
	vm_soperand_t rem = (vm_soperand_t)spad[0] % (vm_soperand_t)spad[1];
	spad[0] = rem;
	spad[1] = quot;
	goto op_push;
}
op_AND:
	spad[0] = spad[0] & spad[1];
	goto op_push;
op_OR:
	spad[0] = spad[0] | spad[1];
	goto op_push;
op_XOR:
	spad[0] = spad[0] ^ spad[1];
	goto op_push;
op_NOT:
	spad[0] = ~spad[0];
	goto op_push;
op_LSL:
	spad[0] = spad[0] << spad[1];
	goto op_push;
op_LSR:
	spad[0] = ((vm_uoperand_t)spad[0]) >> spad[1];
	goto op_push;
op_ASR:
	spad[0] = ((vm_soperand_t)spad[0]) >> spad[1];
op_CC_LT:
	spad[0] = ((vm_soperand_t)spad[0]) < ((vm_soperand_t)spad[1]);
	goto op_push;
op_CC_GT:
	spad[0] = ((vm_soperand_t)spad[0]) > ((vm_soperand_t)spad[1]);
	goto op_push;
op_CC_LE:
	spad[0] = ((vm_soperand_t)spad[0]) <= ((vm_soperand_t)spad[1]);
	goto op_push;
op_CC_GE:
	spad[0] = ((vm_soperand_t)spad[0]) >= ((vm_soperand_t)spad[1]);
	goto op_push;
op_CC_B:
	spad[0] = ((vm_uoperand_t)spad[0]) < ((vm_uoperand_t)spad[1]);
	goto op_push;
op_CC_A:
	spad[0] = ((vm_uoperand_t)spad[0]) > ((vm_uoperand_t)spad[1]);
	goto op_push;
op_CC_BE:
	spad[0] = ((vm_uoperand_t)spad[0]) <= ((vm_uoperand_t)spad[1]);
	goto op_push;
op_CC_AE:
	spad[0] = ((vm_uoperand_t)spad[0]) >= ((vm_uoperand_t)spad[1]);
	goto op_push;
op_CC_EQ:
	spad[0] = spad[0] == spad[1];
	goto op_push;
op_CC_NE:
	spad[0] = spad[0] != spad[1];
	goto op_push;
op_LDC0:
	spad[0] = 0;
	goto op_push;
op_LDC1: 
	spad[0] = 1;
	goto op_push;
op_LDC2: 
	spad[0] = 2;
	goto op_push;
op_LDCU8:
	spad[0] = (vm_uoperand_t)(uint8_t)vm_fetch8(ctx);
	goto op_push;
op_LDCS8:
	spad[0] = (vm_soperand_t)(int8_t)vm_fetch8(ctx);
	goto op_push;
op_LDC32:
	spad[0] = vm_fetch32(ctx);
	goto op_push;
op_LDMU8:
	spad[0] = (vm_uoperand_t)*(uint8_t *)(spad[0]);
	goto op_push;
op_LDMS8:
	spad[0] = (vm_soperand_t)*(int8_t *)(spad[0]);
	goto op_push;
op_LDMU16:
	spad[0] = (vm_uoperand_t)*(uint16_t *)(spad[0]);
	goto op_push;
op_LDMS16:
	spad[0] = (vm_soperand_t)*(int16_t *)(spad[0]);
	goto op_push;
op_LDM32:
	spad[0] = *(vm_operand_t *)(spad[0]);
	goto op_push;
op_STU8:
	*(uint8_t *)(spad[0]) = (uint8_t)spad[1];
	goto op_push;
op_STU16:
	*(uint16_t *)(spad[0]) = (uint16_t)spad[1];
	goto op_push;
op_STM32:
	*(vm_operand_t *)(spad[0]) = spad[1];
	goto op_push;
op_DUP:
	spad[1] = spad[0];
	goto op_push;
op_SWAP: {
	vm_operand_t temp;
	temp = spad[0];
	spad[0] = spad[1];
	spad[1] = temp;
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
	if (spad[0]) {
		ctx->pc += offset;
	}
	goto op_push;
}
op_BRF: {
	int8_t offset = (int8_t)vm_fetch8(ctx);
	if (!spad[0]) {
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
	const vm_thunk_t *thunk;
	int32_t offset = (int32_t)vm_fetch32(ctx);
	uint32_t *hash_ptr = (uint32_t *)(ctx->pc + offset);
	vm_thunk_rv proc;
	vm_operand_t rv;
	vm_verify_ptr(hash_ptr);
	
	thunk = vm_find_thunk(*hash_ptr);
	if (!thunk) {
		vm_panic("vm_step: native call for an unknown function (hash: %08X)", *hash_ptr);
	}
	
	vm_stack_popn(&ctx->dstack, thunk->argc, spad);
	
	proc = (vm_thunk_rv)thunk->target;
	/* In all ABIs I know of, the return value is passed via a register. */
	/* Thus, I assume always asking for one is not an issue. */
	switch (thunk->argc) {
		case 0: rv = proc(); break;
		case 1: rv = proc(spad[0]); break;
		case 2: rv = proc(spad[0], spad[1]); break;
		case 3: rv = proc(spad[0], spad[1], spad[2]); break;
		case 4: rv = proc(spad[0], spad[1], spad[2], spad[3]); break;
		case 5: rv = proc(spad[0], spad[1], spad[2], spad[3], spad[4]); break;
		case 6: rv = proc(spad[0], spad[1], spad[2], spad[3], spad[4], spad[5]); break;
		case 7: rv = proc(spad[0], spad[1], spad[2], spad[3], spad[4], spad[5], spad[6]); break;
	}
	if (thunk->has_rv) {
		vm_stack_push(&ctx->dstack, rv);
	}
	goto op_push;
}
	
op_push:
	sop_count = traits->push_count;
	sop_index = 0;
	while (sop_count) {
		vm_stack_push(&ctx->dstack, spad[sop_index]);
		sop_index++;
		sop_count--;
	}
	
}

