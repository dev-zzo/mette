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
	r1 = (vm_uoperand_t)(a) / (vm_uoperand_t)(b); \
	r0 = (vm_uoperand_t)(a) % (vm_uoperand_t)(b);
#define TARGET_DIVS(a,b) \
	r1 = (vm_soperand_t)(a) / (vm_soperand_t)(b); \
	r0 = (vm_soperand_t)(a) % (vm_soperand_t)(b);
#endif
#if defined(TARGET_ARCH_i386)
#define TARGET_MULU(a,b,c,d) \
	(c) = (a) * (b)
#define TARGET_MULS(a,b,c,d) \
	(c) = (a) * (b)
#define TARGET_DIVU(a,b) \
	r1 = (vm_uoperand_t)(a) / (vm_uoperand_t)(b); \
	r0 = (vm_uoperand_t)(a) % (vm_uoperand_t)(b);
#define TARGET_DIVS(a,b) \
	r1 = (vm_soperand_t)(a) / (vm_soperand_t)(b); \
	r0 = (vm_soperand_t)(a) % (vm_soperand_t)(b);
#endif

typedef struct _vm_callframe_t vm_callframe_t;
struct _vm_callframe_t {
	uint8_t *return_pc;
	vm_locals_t *locals;
	vm_operand_t *dstack_top; /* For debugging */
};

static void vm_save_frame(vm_context_t *ctx, uint8_t *pc)
{
	vm_callframe_t *frame;

	frame = vm_alloc(sizeof(*frame));
	frame->return_pc = pc;
	frame->locals = ctx->locals;
	frame->dstack_top = vm_stack_top(ctx->dstack);

	vm_stack_push(ctx->cstack, (vm_operand_t)frame);

	ctx->locals = NULL;
}

static uint8_t *vm_restore_frame(vm_context_t *ctx)
{
	vm_callframe_t *frame;
	uint8_t *pc;

	vm_free(ctx->locals);
	frame = (vm_callframe_t *)vm_stack_pop(ctx->cstack);
	ctx->locals = frame->locals;
	pc = frame->return_pc;
	vm_free(frame);

	return pc;
}

#ifdef DEBUG_PRINTS
#include "vm_opcodes.names.tab"
#endif

#define ARG1 (buf[0])
#define ARG2 (buf[1])

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
	r0 = ARG2 + ARG1;
	goto push_1;
op_SUB:
	r0 = ARG2 - ARG1;
	goto push_1;
op_MULU: {
	TARGET_MULU(ARG1, ARG2, r0, r1);
	goto push_2;
}
op_MULS: {
	TARGET_MULS(ARG1, ARG2, r0, r1);
	goto push_2;
}
op_DIVU: {
	TARGET_DIVU(ARG2, ARG1);
	goto push_2;
}
op_DIVS: {
	TARGET_DIVS(ARG2, ARG1);
	goto push_2;
}
op_AND:
	r0 = ARG1 & ARG2;
	goto push_1;
op_OR:
	r0 = ARG1 | ARG2;
	goto push_1;
op_XOR:
	r0 = ARG1 ^ ARG2;
	goto push_1;
op_NOT:
	r0 = ~ARG1;
	goto push_1;
op_LSL:
	r0 = ARG2 << ARG1;
	goto push_1;
op_LSR:
	r0 = ((vm_uoperand_t)ARG2) >> ARG1;
	goto push_1;
op_ASR:
	r0 = ((vm_soperand_t)ARG2) >> ARG1;
	goto push_1;
op_CMP_LT:
	r0 = ((vm_soperand_t)ARG1) < ((vm_soperand_t)ARG2);
	goto push_1;
op_CMP_GT:
	r0 = ((vm_soperand_t)ARG1) > ((vm_soperand_t)ARG2);
	goto push_1;
op_CMP_B:
	r0 = ((vm_uoperand_t)ARG1) < ((vm_uoperand_t)ARG2);
	goto push_1;
op_CMP_A:
	r0 = ((vm_uoperand_t)ARG1) > ((vm_uoperand_t)ARG2);
	goto push_1;
op_CMP_EQ:
	r0 = ARG2 == ARG1;
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
op_LDC_UB:
	r0 = (vm_uoperand_t)*(uint8_t *)(pc);
	pc += 1;
	goto push_1;
op_LDC_SB:
	r0 = (vm_soperand_t)*(int8_t *)(pc);
	pc += 1;
	goto push_1;
op_LDC_W:
	r0 = vm_fetch32_ua(pc);
	pc += 4;
	goto push_1;
op_LEA:
	r0 = vm_fetch32_ua(pc);
	DBGPRINT("vm_step: PCREL offset %08x -> ", r0);
	pc += 4;
	r0 += (vm_uoperand_t)pc;
	DBGPRINT("%08x\n", r0);
	goto push_1;
op_LDM_UB:
	r0 = (vm_uoperand_t)*(uint8_t *)(ARG1);
	goto push_1;
op_LDM_SB:
	r0 = (vm_soperand_t)*(int8_t *)(ARG1);
	goto push_1;
op_LDM_UH:
	r0 = (vm_uoperand_t)*(uint16_t *)(ARG1);
	goto push_1;
op_LDM_SH:
	r0 = (vm_soperand_t)*(int16_t *)(ARG1);
	goto push_1;
op_LDM_W:
	r0 = *(vm_operand_t *)(ARG1);
	goto push_1;
op_STM_B:
	*(uint8_t *)(ARG1) = (uint8_t)ARG2;
	DBGPRINT("vm_step: %08x -> %08x\n", ARG2, ARG1);
	goto push_none;
op_STM_H:
	*(uint16_t *)(ARG1) = (uint16_t)ARG2;
	DBGPRINT("vm_step: %08x -> %08x\n", ARG2, ARG1);
	goto push_none;
op_STM_W:
	*(vm_operand_t *)(ARG1) = ARG2;
	DBGPRINT("vm_step: %08x -> %08x\n", ARG2, ARG1);
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
	ctx->locals->data[index] = ARG1;
	goto push_none;
}
op_DUP:
	r1 = r0 = ARG1;
	goto push_2;
op_SWAP: {
	r1 = ARG1;
	r0 = ARG2;
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
	if (ARG1) {
		pc = pc + offset;
	}
	goto push_none;
}
op_BR_F: {
	int8_t offset = *(int8_t *)(pc);
	pc += 1;
	if (!ARG1) {
		pc = pc + offset;
	}
	goto push_none;
}
op_CALL: {
	int32_t offset = (int32_t)vm_fetch32_ua(pc);
	pc += 4;
	vm_save_frame(ctx, pc);
	pc += offset;
	DBGPRINT("\n");
	goto push_none;
}
op_RET: {
	vm_callframe_t *frame;

	vm_free(ctx->locals);
	frame = (vm_callframe_t *)vm_stack_pop(ctx->cstack);
	ctx->locals = frame->locals;
	pc = frame->return_pc;

	DBGPRINT("vm_step: stack balance on return: %d\n\n", frame->dstack_top - vm_stack_top(ctx->dstack));
	vm_free(frame);
	goto push_none;
}
op_ICALL: {
	vm_save_frame(ctx, pc);
	pc = (uint8_t *)ARG1;
	goto push_none;
}
op_IJMP: {
	pc = (uint8_t *)ARG1;
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
	vm_stack_push(ctx->dstack, r1);
push_1:
	vm_stack_push(ctx->dstack, r0);
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
