#include "vm_internal.h"
#include "vm_opcodes.h"
#include "vm_opcodes_traits.h"
#include "vm_thunks.h"
#include "vm_loader.h"
#include "vm_misc.h"

/* Assembly hacks (platform specific) thanks to GCC. */
#if defined(TARGET_ARCH_mips)
#define TARGET_MULU(a,b) \
	__asm__ __volatile__( \
		"multu %0, %1\n\t" \
		"mfhi %0\n\t" \
		"mflo %1\n\t" \
		: "=r"(a), "=r"(b) \
		)
#define TARGET_MULS(a,b) \
	__asm__ __volatile__( \
		"mult %0, %1\n\t" \
		"mfhi %0\n\t" \
		"mflo %1\n\t" \
		: "=r"(a), "=r"(b) \
		)
#define TARGET_DIVU(a,b) \
	vm_uoperand_t quot = (vm_uoperand_t)op0 / (vm_uoperand_t)op1; \
	vm_uoperand_t rem = (vm_uoperand_t)op0 % (vm_uoperand_t)op1; \
	op0 = rem; \
	op1 = quot;
#define TARGET_DIVS(a,b) \
	vm_soperand_t quot = (vm_soperand_t)op0 / (vm_soperand_t)op1; \
	vm_soperand_t rem = (vm_soperand_t)op0 % (vm_soperand_t)op1; \
	op0 = rem; \
	op1 = quot;
#endif

static uint8_t vm_fetch8_pc(vm_context_t *ctx)
{
	uint8_t v = *ctx->pc;
	ctx->pc += 1;
	return v;
}

static uint16_t vm_fetch16_pc(vm_context_t *ctx)
{
	uint16_t v = vm_fetch16_ua(ctx->pc);
	ctx->pc += sizeof(uint16_t);
	return v;
}

static uint32_t vm_fetch32_pc(vm_context_t *ctx)
{
	uint32_t v = vm_fetch32_ua(ctx->pc);
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
	
	opcode = (vm_opcode_t)vm_fetch8_pc(ctx);
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
	TARGET_MULU(op0, op1);
	goto push_2;
}
op_MULS: {
	TARGET_MULS(op0, op1);
	goto push_2;
}
op_DIVU: {
	TARGET_DIVU(op0, op1);
	goto push_2;
}
op_DIVS: {
	TARGET_DIVS(op0, op1);
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
	op0 = (vm_uoperand_t)(uint8_t)vm_fetch8_pc(ctx);
	goto push_1;
op_LDCS8:
	op0 = (vm_soperand_t)(int8_t)vm_fetch8_pc(ctx);
	goto push_1;
op_LDC32:
	op0 = vm_fetch32_pc(ctx);
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
	int8_t offset = (int8_t)vm_fetch8_pc(ctx);
	ctx->pc += offset;
	goto push_none;
}
op_BRL: {
	int32_t offset = (int32_t)vm_fetch32_pc(ctx);
	ctx->pc += offset;
	goto push_none;
}
op_BRT: {
	int8_t offset = (int8_t)vm_fetch8_pc(ctx);
	if (op0) {
		ctx->pc += offset;
	}
	goto push_none;
}
op_BRF: {
	int8_t offset = (int8_t)vm_fetch8_pc(ctx);
	if (!op0) {
		ctx->pc += offset;
	}
	goto push_none;
}
op_CALL: {
	int32_t offset = (int32_t)vm_fetch32_pc(ctx);
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
	int index = vm_fetch16_pc(ctx); /* thunk index */
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


