#include "vm_internal.h"
#include "vm_opcodes.h"
#include "vm_thunks.h"
#include "vm_loader.h"
#include "vm_misc.h"

/* Assembly hacks (platform specific) thanks to GCC. */
#if defined(TARGET_ARCH_mips)
#define TARGET_MULU(a,b,c,d) \
	__asm__ __volatile__( \
		"multu %0, %1\n\t" \
		"mfhi %2\n\t" \
		"mflo %3\n\t" \
		: "=r"(c), "=r"(d) \
		: "r"(a), "r"(b) \
		)
#define TARGET_MULS(a,b,c,d) \
	__asm__ __volatile__( \
		"mult %0, %1\n\t" \
		"mfhi %2\n\t" \
		"mflo %3\n\t" \
		: "=r"(c), "=r"(d) \
		: "r"(a), "r"(b) \
		)
#define TARGET_DIVU(a,b) \
	vm_uoperand_t quot = (vm_uoperand_t)op0 / (vm_uoperand_t)op1; \
	vm_uoperand_t rem = (vm_uoperand_t)op0 % (vm_uoperand_t)op1; \
	r0 = rem; \
	r1 = quot;
#define TARGET_DIVS(a,b) \
	vm_soperand_t quot = (vm_soperand_t)op0 / (vm_soperand_t)op1; \
	vm_soperand_t rem = (vm_soperand_t)op0 % (vm_soperand_t)op1; \
	r0 = rem; \
	r1 = quot;
#endif

/*
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
*/

/* Emulate one VM insn. */
void vm_step(vm_context_t *ctx)
{
#include "vm_opcodes_switch.tab"
	vm_operand_t op0, op1, op2, op3;
	vm_operand_t r0, r1, r2, r3;
	uint8_t *pc = ctx->pc;
	unsigned opcount;
	vm_opcode_t opcode;
	
	opcode = (const vm_opcode_t)(*pc);
	pc += 1;
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
	r0 = op0 + op1;
	goto push_1;
op_SUB:
	r0 = op0 - op1;
	goto push_1;
op_MULU: {
	TARGET_MULU(op0, op1, r0, r1);
	goto push_2;
}
op_MULS: {
	TARGET_MULS(op0, op1, r0, r1);
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
	r0 = op0 & op1;
	goto push_1;
op_OR:
	r0 = op0 | op1;
	goto push_1;
op_XOR:
	r0 = op0 ^ op1;
	goto push_1;
op_NOT:
	r0 = ~op0;
	goto push_1;
op_LSL:
	r0 = op0 << op1;
	goto push_1;
op_LSR:
	r0 = ((vm_uoperand_t)op0) >> op1;
	goto push_1;
op_ASR:
	r0 = ((vm_soperand_t)op0) >> op1;
	goto push_1;
op_CMP_LT:
	r0 = ((vm_soperand_t)op0) < ((vm_soperand_t)op1);
	goto push_1;
op_CMP_GT:
	r0 = ((vm_soperand_t)op0) > ((vm_soperand_t)op1);
	goto push_1;
op_CMP_LE:
	r0 = ((vm_soperand_t)op0) <= ((vm_soperand_t)op1);
	goto push_1;
op_CMP_GE:
	r0 = ((vm_soperand_t)op0) >= ((vm_soperand_t)op1);
	goto push_1;
op_CMP_B:
	r0 = ((vm_uoperand_t)op0) < ((vm_uoperand_t)op1);
	goto push_1;
op_CMP_A:
	r0 = ((vm_uoperand_t)op0) > ((vm_uoperand_t)op1);
	goto push_1;
op_CMP_BE:
	r0 = ((vm_uoperand_t)op0) <= ((vm_uoperand_t)op1);
	goto push_1;
op_CMP_AE:
	r0 = ((vm_uoperand_t)op0) >= ((vm_uoperand_t)op1);
	goto push_1;
op_CMP_EQ:
	r0 = op0 == op1;
	goto push_1;
op_CMP_NE:
	r0 = op0 != op1;
	goto push_1;
op_LDC_0:
	r0 = 0;
	goto push_1;
op_LDC_1: 
	r0 = 1;
	goto push_1;
op_LDC_2: 
	r0 = 2;
	goto push_1;
op_LDC_8_U:
	r0 = (vm_uoperand_t)*(uint8_t *)(pc);
	pc += 1;
	goto push_1;
op_LDC_8_S:
	r0 = (vm_soperand_t)*(int8_t *)(pc);
	pc += 1;
	goto push_1;
op_LDC_32:
	r0 = vm_fetch32_ua(pc);
	pc += 4;
	goto push_1;
op_LDM_8_U:
	r0 = (vm_uoperand_t)*(uint8_t *)(op0);
	goto push_1;
op_LDM_8_S:
	r0 = (vm_soperand_t)*(int8_t *)(op0);
	goto push_1;
op_LDM_16_U:
	r0 = (vm_uoperand_t)*(uint16_t *)(op0);
	goto push_1;
op_LDM_16_S:
	r0 = (vm_soperand_t)*(int16_t *)(op0);
	goto push_1;
op_LDM_32:
	r0 = *(vm_operand_t *)(op0);
	goto push_1;
op_STM_8:
	*(uint8_t *)(op0) = (uint8_t)op1;
	goto push_none;
op_STM_16:
	*(uint16_t *)(op0) = (uint16_t)op1;
	goto push_none;
op_STM_32:
	*(vm_operand_t *)(op0) = op1;
	goto push_none;
op_LOCALS: {
	uint8_t count = *(uint8_t *)(pc);
	ctx->locals = (vm_locals_t *)vm_alloc(sizeof(vm_locals_t) + (count - 1) * sizeof(vm_operand_t));
	ctx->locals->count = count;
	goto push_none;
}
op_LDLOC: {
	uint8_t index = *(uint8_t *)(pc);
	pc += 1;
	if (!ctx->locals) {
		vm_panic("vm_step: accessing locals where none have been allocated.");
	}
	if (index >= ctx->locals->count) {
		vm_panic("vm_step: local index out of bounds.");
	}
	r0 = ctx->locals->data[index];
	goto push_1;
}
op_STLOC: {
	uint8_t index = *(uint8_t *)(pc);
	pc += 1;
	if (!ctx->locals) {
		vm_panic("vm_step: accessing locals where none have been allocated.");
	}
	if (index >= ctx->locals->count) {
		vm_panic("vm_step: local index out of bounds.");
	}
	ctx->locals->data[index] = op0;
	goto push_none;
}
op_DUP:
	r1 = r0 = op0;
	goto push_2;
op_SWAP: {
	r1 = op0;
	r0 = op1;
	goto push_2;
}
op_POP:
	goto push_none;
op_BR_S: {
	int8_t offset = *(int8_t *)(pc);
	pc = pc + 1 + offset;
	goto push_none;
}
op_BR_L: {
	int32_t offset = (int32_t)vm_fetch32_ua(pc);
	pc = pc + 4 + offset;
	goto push_none;
}
op_BR_T: {
	int8_t offset = *(int8_t *)(pc);
	if (op0) {
		pc = pc + 1 + offset;
	}
	goto push_none;
}
op_BR_F: {
	int8_t offset = *(int8_t *)(pc);
	if (!op0) {
		pc = pc + 1 + offset;
	}
	goto push_none;
}
op_CALL: {
	int32_t offset = (int32_t)vm_fetch32_ua(pc);
	pc += 4;
	vm_stack_push(&ctx->cstack, (vm_operand_t)pc);
	vm_stack_push(&ctx->cstack, (vm_operand_t)ctx->locals);
	ctx->locals = NULL;
	pc += offset;
	goto push_none;
}
op_RET: {
	if (ctx->locals) {
		vm_free(ctx->locals);
	}
	ctx->locals = (void *)vm_stack_pop(&ctx->cstack);
	pc = (uint8_t *)vm_stack_pop(&ctx->cstack);
	goto push_none;
}
op_ICALL: {
	vm_stack_push(&ctx->cstack, (vm_operand_t)pc);
	vm_stack_push(&ctx->cstack, (vm_operand_t)ctx->locals);
	ctx->locals = NULL;
	pc = (uint8_t *)op0;
	goto push_none;
}
op_IJMP: {
	pc = (uint8_t *)op0;
	goto push_none;
}
op_NCALL: {
	int index = vm_fetch16_ua(pc); /* thunk index */
	pc += 2;
	vm_thunk_t thunk = (vm_thunk_t)ctx->module->ncalltab[index];
	thunk(ctx);
	goto push_none;
}
	
push_2:
	vm_stack_push(&ctx->dstack, r1);
push_1:
	vm_stack_push(&ctx->dstack, r0);
push_none:
	
	ctx->pc = pc;
	return;
}

int vm_load_exec(const char *path)
{
	int fd;
	vm_module_t *module;
	vm_context_t *context;
	
	fd = vm_open(path, O_RDONLY, 0);
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


