#include "vm_internal.h"
#include "vm_thunks.h"
#include "vm_loader.h"
#include "vm_misc.h"

#define DEBUG_PRINTS
#include "rtl_debug.h"

/* Assembly hacks (platform specific) thanks to GCC. */
#if defined(TARGET_ARCH_mips)
#define TARGET_MULU(a,b,c,d) \
	__asm__ __volatile__( \
		"multu %2, %3\n\t" \
		"mfhi %0\n\t" \
		"mflo %1\n\t" \
		: "=r"(c), "=r"(d) \
		: "r"(a), "r"(b) \
		)
#define TARGET_MULS(a,b,c,d) \
	__asm__ __volatile__( \
		"mult %2, %3\n\t" \
		"mfhi %0\n\t" \
		"mflo %1\n\t" \
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

#ifdef DEBUG_PRINTS
#include "vm_opcodes.names.tab"
#endif

/* Emulate one VM insn. */
void vm_step(vm_context_t *ctx)
{
#include "vm_opcodes.switch.tab"
	vm_operand_t op0, op1, op2, op3;
	vm_operand_t r0, r1, r2, r3;
	uint8_t *pc = ctx->pc;
	unsigned opcount;
	uint8_t opcode;
	
	opcode = *pc;
	DBGPRINT("vm_step: %08x -> %02x ", pc, opcode);
	pc += 1;
	opcount = opcode >> 6; /* 2 highest bits make for operand count. */
	opcode &= 0x3F;
	DBGPRINT("(%s / %d)\n", vm_insn_to_name[opcode], opcount);

	switch (opcount) {
	case 2:
		op1 = vm_stack_pop(&ctx->dstack);
	case 1:
		op0 = vm_stack_pop(&ctx->dstack);
	case 0:
		break;
	}
	
	goto *(&&op_invalid + offtab[opcode]);
	
op_invalid:
	vm_panic("vm_step: unknown opcode.");

op_ADD:
	r0 = op1 + op0;
	goto push_1;
op_SUB:
	r0 = op1 - op0;
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
	r0 = op1 & op0;
	goto push_1;
op_OR:
	r0 = op1 | op0;
	goto push_1;
op_XOR:
	r0 = op1 ^ op0;
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
	r0 = ((vm_soperand_t)op1) < ((vm_soperand_t)op0);
	goto push_1;
op_CMP_GT:
	r0 = ((vm_soperand_t)op1) > ((vm_soperand_t)op0);
	DBGPRINT("op_CMP_GT: %d > %d = %d\n", op1, op0, r0);
	goto push_1;
op_CMP_B:
	r0 = ((vm_uoperand_t)op1) < ((vm_uoperand_t)op0);
	goto push_1;
op_CMP_A:
	r0 = ((vm_uoperand_t)op1) > ((vm_uoperand_t)op0);
	goto push_1;
op_CMP_EQ:
	r0 = op1 == op0;
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
op_LEA:
	r0 = vm_fetch32_ua(pc);
	pc += 4;
	r0 += (vm_uoperand_t)pc;
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
	*(uint8_t *)(op1) = (uint8_t)op0;
	goto push_none;
op_STM_16:
	*(uint16_t *)(op1) = (uint16_t)op0;
	goto push_none;
op_STM_32:
	*(vm_operand_t *)(op1) = op0;
	goto push_none;
op_LOCALS: {
	uint8_t count = *(uint8_t *)(pc);
	pc += 1;
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
	r1 = op1;
	r0 = op0;
	goto push_2;
}
op_POP:
	goto push_none;
op_BR: {
	int8_t offset = *(int8_t *)(pc);
	pc += 1;
	pc = pc + offset;
	goto push_none;
}
op_BR_T: {
	int8_t offset = *(int8_t *)(pc);
	pc += 1;
	if (op0) {
		pc = pc + offset;
	}
	goto push_none;
}
op_BR_F: {
	int8_t offset = *(int8_t *)(pc);
	pc += 1;
	if (!op0) {
		pc = pc + offset;
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
	if (!ctx->module->ncalls_table) {
		vm_panic("vm_step: no ncalls defined, but a ncall insn encountered.");
	}
	vm_thunk_t thunk = (vm_thunk_t)ctx->module->ncalls_table[index];
	DBGPRINT("vm_step: NCALL: calling %d @%08X\n", index, thunk);
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

vm_context_t *vm_context_create(vm_module_t *module)
{
	vm_context_t *ctx;

	ctx = (vm_context_t *)vm_alloc(sizeof(*ctx));
	if (!ctx) {
		vm_panic("vm_context_create: failed to allocate context.");
	}
	
	vm_stack_init(&ctx->dstack);
	vm_stack_init(&ctx->cstack);
	ctx->pc = module->entry;
	ctx->locals = NULL;
	ctx->module = module;

	return ctx;
}
