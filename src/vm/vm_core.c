#include "vm_internal.h"
#include "vm_opcodes.h"
#include "vm_opcodes_traits.h"
#include "vm_thunks.h"
#include "vm_loader.h"
#include "vm_misc.h"

static uint8_t vm_fetch8(vm_context_t *ctx)
{
	uint8_t v = *ctx->pc;
	ctx->pc += 1;
	return v;
}

static uint16_t vm_fetch16(vm_context_t *ctx )
{
#ifdef TARGET_IS_BE
	uint16_t v = ctx->pc[1] | (ctx->pc[0] << 8);
#else
	uint16_t v = ctx->pc[0] | (ctx->pc[1] << 8);
#endif
	ctx->pc += sizeof(uint16_t);
	return v;
}

static uint32_t vm_fetch32(vm_context_t *ctx)
{
	uint32_t v = VM_LE32(*(uint32_t *)(ctx->pc));
	ctx->pc += sizeof(uint32_t);
	return v;
}

/* Emulate one VM insn. */
void vm_step(vm_context_t *ctx)
{
#include "vm_opcodes_switch.tab"
	vm_operand_t op0, op1, op2, op3;
	unsigned opcount;
	vm_opcode_t opcode;
	
	opcode = (vm_opcode_t)vm_fetch8(ctx);
	opcount = opcode >> 6; /* 2 highest bits make for operand count. */

	switch (opcount) {
	case 2:
		op1 = vm_stack_pop(&ctx->dstack);
	case 1:
		op0 = vm_stack_pop(&ctx->dstack);
	case 0:
		break;
	}
	
	goto *(&&op_invalid + offtab[opcode & 0x3F]);
	
op_invalid:
	vm_panic("vm_step: unknown opcode.");

op_ADD:
	op0 = op0 + op1;
	goto push_1;
op_SUB:
	op0 = op0 - op1;
	goto push_1;
op_MULU: {
	uint64_t res = (uint64_t)op0 * (uint64_t)op1;
	op0 = res >> 32;
	op1 = res & 0xFFFFFFFFULL;
	goto push_2;
}
op_MULS: {
	int64_t res = ((int64_t)op0) * ((int64_t)op1);
	op0 = res >> 32;
	op1 = (vm_operand_t)res;
	goto push_2;
}
op_DIVU: {
	vm_uoperand_t quot = (vm_uoperand_t)op0 / (vm_uoperand_t)op1;
	vm_uoperand_t rem = (vm_uoperand_t)op0 % (vm_uoperand_t)op1;
	op0 = rem;
	op1 = quot;
	goto push_2;
}
op_DIVS: {
	vm_soperand_t quot = (vm_soperand_t)op0 / (vm_soperand_t)op1;
	vm_soperand_t rem = (vm_soperand_t)op0 % (vm_soperand_t)op1;
	op0 = rem;
	op1 = quot;
	goto push_2;
}
op_AND:
	op0 = op0 & op1;
	goto push_1;
op_OR:
	op0 = op0 | op1;
	goto push_1;
op_XOR:
	op0 = op0 ^ op1;
	goto push_1;
op_NOT:
	op0 = ~op0;
	goto push_1;
op_LSL:
	op0 = op0 << op1;
	goto push_1;
op_LSR:
	op0 = ((vm_uoperand_t)op0) >> op1;
	goto push_1;
op_ASR:
	op0 = ((vm_soperand_t)op0) >> op1;
	goto push_1;
op_CC_LT:
	op0 = ((vm_soperand_t)op0) < ((vm_soperand_t)op1);
	goto push_1;
op_CC_GT:
	op0 = ((vm_soperand_t)op0) > ((vm_soperand_t)op1);
	goto push_1;
op_CC_LE:
	op0 = ((vm_soperand_t)op0) <= ((vm_soperand_t)op1);
	goto push_1;
op_CC_GE:
	op0 = ((vm_soperand_t)op0) >= ((vm_soperand_t)op1);
	goto push_1;
op_CC_B:
	op0 = ((vm_uoperand_t)op0) < ((vm_uoperand_t)op1);
	goto push_1;
op_CC_A:
	op0 = ((vm_uoperand_t)op0) > ((vm_uoperand_t)op1);
	goto push_1;
op_CC_BE:
	op0 = ((vm_uoperand_t)op0) <= ((vm_uoperand_t)op1);
	goto push_1;
op_CC_AE:
	op0 = ((vm_uoperand_t)op0) >= ((vm_uoperand_t)op1);
	goto push_1;
op_CC_EQ:
	op0 = op0 == op1;
	goto push_1;
op_CC_NE:
	op0 = op0 != op1;
	goto push_1;
op_LDC0:
	op0 = 0;
	goto push_1;
op_LDC1: 
	op0 = 1;
	goto push_1;
op_LDC2: 
	op0 = 2;
	goto push_1;
op_LDCU8:
	op0 = (vm_uoperand_t)(uint8_t)vm_fetch8(ctx);
	goto push_1;
op_LDCS8:
	op0 = (vm_soperand_t)(int8_t)vm_fetch8(ctx);
	goto push_1;
op_LDC32:
	op0 = vm_fetch32(ctx);
	goto push_1;
op_LDMU8:
	op0 = (vm_uoperand_t)*(uint8_t *)(op0);
	goto push_1;
op_LDMS8:
	op0 = (vm_soperand_t)*(int8_t *)(op0);
	goto push_1;
op_LDMU16:
	op0 = (vm_uoperand_t)*(uint16_t *)(op0);
	goto push_1;
op_LDMS16:
	op0 = (vm_soperand_t)*(int16_t *)(op0);
	goto push_1;
op_LDM32:
	op0 = *(vm_operand_t *)(op0);
	goto push_1;
op_STM8:
	*(uint8_t *)(op0) = (uint8_t)op1;
	goto push_none;
op_STM16:
	*(uint16_t *)(op0) = (uint16_t)op1;
	goto push_none;
op_STM32:
	*(vm_operand_t *)(op0) = op1;
	goto push_none;
op_LOCALS: {
	ctx->locals = (vm_locals_t *)vm_alloc(sizeof(vm_locals_t) + (op0 - 1) * sizeof(vm_operand_t));
	ctx->locals->count = op0;
	goto push_none;
}
op_LDLOC: {
	if (!ctx->locals) {
		vm_panic("vm_step: accessing locals where none have been allocated.");
	}
	if (op0 >= ctx->locals->count) {
		vm_panic("vm_step: local index out of bounds.");
	}
	op0 = ctx->locals->data[op0];
	goto push_1;
}
op_STLOC: {
	if (!ctx->locals) {
		vm_panic("vm_step: accessing locals where none have been allocated.");
	}
	if (op0 >= ctx->locals->count) {
		vm_panic("vm_step: local index out of bounds.");
	}
	ctx->locals->data[op0] = op1;
	goto push_none;
}
op_DUP:
	op1 = op0;
	goto push_2;
op_SWAP: {
	vm_operand_t temp;
	temp = op0;
	op0 = op1;
	op1 = temp;
	goto push_2;
}
op_POP:
	goto push_none;
op_BRS: {
	int8_t offset = (int8_t)vm_fetch8(ctx);
	ctx->pc += offset;
	goto push_none;
}
op_BRL: {
	int32_t offset = (int32_t)vm_fetch32(ctx);
	ctx->pc += offset;
	goto push_none;
}
op_BRT: {
	int8_t offset = (int8_t)vm_fetch8(ctx);
	if (op0) {
		ctx->pc += offset;
	}
	goto push_none;
}
op_BRF: {
	int8_t offset = (int8_t)vm_fetch8(ctx);
	if (!op0) {
		ctx->pc += offset;
	}
	goto push_none;
}
op_CALL: {
	int32_t offset = (int32_t)vm_fetch32(ctx);
	vm_stack_push(&ctx->cstack, (vm_operand_t)ctx->pc);
	vm_stack_push(&ctx->cstack, (vm_operand_t)ctx->locals);
	ctx->pc += offset;
	ctx->locals = NULL;
	goto push_none;
}
op_RET: {
	if (ctx->locals) {
		vm_free(ctx->locals);
	}
	ctx->locals = (void *)vm_stack_pop(&ctx->cstack);
	ctx->pc = (void *)vm_stack_pop(&ctx->cstack);
	goto push_none;
}
op_ICALL: {
	goto push_none;
}
op_IJMP: {
	goto push_none;
}
op_NCALL: {
	int index = vm_fetch16(ctx); /* thunk index */
	vm_thunk_t thunk = (vm_thunk_t)ctx->module->ncalltab[index];
	thunk(ctx);
	goto push_none;
}
	
push_2:
	vm_stack_push(&ctx->dstack, op1);
push_1:
	vm_stack_push(&ctx->dstack, op0);
push_none:
	
	return;
}

int vm_load_exec(const char *path)
{
	int fd;
	vm_module_t *module;
	vm_context_t *context;
	
	fd = vm_open(path, O_RDONLY);
	if (fd < 0) {
		vm_panic("vm_load_exec: failed to open the image.");
	}
	
	module = vm_load_fd(fd);
	vm_close(fd);
	if (!module) {
		vm_panic("vm_load_exec: failed to load the image.");
	}
	
	context = (vm_context_t *)vm_alloc(sizeof(vm_context_t));
	if (!context) {
		vm_panic("vm_load_exec: failed to allocate context.");
	}
	
	context->pc = module->entry;
	context->module = module;
	context->is_running = 1;
	
	while (context->is_running) {
		vm_step(context);
	}
	
	return 0;
}


