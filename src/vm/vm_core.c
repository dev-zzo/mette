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
	r0 = (vm_uoperand_t)(a) / (vm_uoperand_t)(b); \
	r1 = (vm_uoperand_t)(a) % (vm_uoperand_t)(b);
#define TARGET_DIVS(a,b) \
	r0 = (vm_soperand_t)(a) / (vm_soperand_t)(b); \
	r1 = (vm_soperand_t)(a) % (vm_soperand_t)(b);
#endif

#ifdef DEBUG_PRINTS
#include "vm_opcodes.names.tab"
#endif

/* Emulate one VM insn. */
void vm_step(vm_context_t *ctx)
{
#include "vm_opcodes.switch.tab"
	vm_operand_t r0, r1;
	vm_operand_t buf[3];
	uint8_t *pc = ctx->pc;
	unsigned ops_in, ops_out = 0;
	uint8_t opcode;
	
	opcode = *pc;
	DBGPRINT("vm_step: %08x -> %02x ", pc, opcode);
	pc += 1;
	ops_in = opcode >> 6; /* 2 highest bits make for operand count. */
	opcode &= 0x3F;
	DBGPRINT("(%s / %d)\n", vm_insn_to_name[opcode], ops_in);
	vm_stack_pop3(ctx->dstack, buf, ops_in);

	goto *(&&op_invalid + offtab[opcode]);
	
op_invalid:
	vm_panic("vm_step: unknown opcode.");

op_ADD:
	r0 = buf[0] + buf[1];
	goto push_1;
op_SUB:
	r0 = buf[0] - buf[1];
	goto push_1;
op_MULU: {
	TARGET_MULU(buf[0], buf[1], r0, r1);
	goto push_2;
}
op_MULS: {
	TARGET_MULS(buf[0], buf[1], r0, r1);
	goto push_2;
}
op_DIVU: {
	TARGET_DIVU(buf[0], buf[1]);
	goto push_2;
}
op_DIVS: {
	TARGET_DIVS(buf[0], buf[1]);
	goto push_2;
}
op_AND:
	r0 = buf[0] & buf[1];
	goto push_1;
op_OR:
	r0 = buf[0] | buf[1];
	goto push_1;
op_XOR:
	r0 = buf[0] ^ buf[1];
	goto push_1;
op_NOT:
	r0 = ~buf[0];
	goto push_1;
op_LSL:
	r0 = buf[1] << buf[0];
	goto push_1;
op_LSR:
	r0 = ((vm_uoperand_t)buf[1]) >> buf[0];
	goto push_1;
op_ASR:
	r0 = ((vm_soperand_t)buf[1]) >> buf[0];
	goto push_1;
op_CMP_LT:
	r0 = ((vm_soperand_t)buf[0]) < ((vm_soperand_t)buf[1]);
	goto push_1;
op_CMP_GT:
	r0 = ((vm_soperand_t)buf[0]) > ((vm_soperand_t)buf[1]);
	goto push_1;
op_CMP_B:
	r0 = ((vm_uoperand_t)buf[0]) < ((vm_uoperand_t)buf[1]);
	goto push_1;
op_CMP_A:
	r0 = ((vm_uoperand_t)buf[0]) > ((vm_uoperand_t)buf[1]);
	goto push_1;
op_CMP_EQ:
	r0 = buf[1] == buf[0];
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
	r0 = (vm_uoperand_t)*(uint8_t *)(buf[0]);
	goto push_1;
op_LDM_8_S:
	r0 = (vm_soperand_t)*(int8_t *)(buf[0]);
	goto push_1;
op_LDM_16_U:
	r0 = (vm_uoperand_t)*(uint16_t *)(buf[0]);
	goto push_1;
op_LDM_16_S:
	r0 = (vm_soperand_t)*(int16_t *)(buf[0]);
	goto push_1;
op_LDM_32:
	r0 = *(vm_operand_t *)(buf[0]);
	goto push_1;
op_STM_8:
	*(uint8_t *)(buf[0]) = (uint8_t)buf[1];
	goto push_none;
op_STM_16:
	*(uint16_t *)(buf[0]) = (uint16_t)buf[1];
	goto push_none;
op_STM_32:
	*(vm_operand_t *)(buf[0]) = buf[1];
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
	ctx->locals->data[index] = buf[0];
	goto push_none;
}
op_DUP:
	r1 = r0 = buf[0];
	goto push_2;
op_SWAP: {
	r1 = buf[0];
	r0 = buf[1];
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
	if (buf[0]) {
		pc = pc + offset;
	}
	goto push_none;
}
op_BR_F: {
	int8_t offset = *(int8_t *)(pc);
	pc += 1;
	if (!buf[0]) {
		pc = pc + offset;
	}
	goto push_none;
}
op_CALL: {
	int32_t offset = (int32_t)vm_fetch32_ua(pc);
	pc += 4;
	vm_stack_push(ctx->cstack, (vm_operand_t)pc);
	vm_stack_push(ctx->cstack, (vm_operand_t)ctx->locals);
	ctx->locals = NULL;
	pc += offset;
	goto push_none;
}
op_RET: {
	if (ctx->locals) {
		vm_free(ctx->locals);
	}
	vm_stack_pop3(ctx->cstack, buf, 2);
	ctx->locals = (void *)buf[0];
	pc = (uint8_t *)buf[1];
	goto push_none;
}
op_ICALL: {
	vm_stack_push(ctx->cstack, (vm_operand_t)pc);
	vm_stack_push(ctx->cstack, (vm_operand_t)ctx->locals);
	ctx->locals = NULL;
	pc = (uint8_t *)buf[0];
	goto push_none;
}
op_IJMP: {
	pc = (uint8_t *)buf[0];
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
	buf[1] = r1;
	ops_out++;
push_1:
	buf[0] = r0;
	ops_out++;
	vm_stack_push3(ctx->dstack, buf, ops_out);
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

	ctx->dstack = vm_stack_create(65536);
	ctx->cstack = vm_stack_create(8192);	
	ctx->pc = module->entry;
	ctx->locals = NULL;
	ctx->module = module;

	return ctx;
}
