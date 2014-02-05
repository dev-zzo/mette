#ifndef __mette_vm_thunks_h_included
#define __mette_vm_thunks_h_included

#include "vm_internal.h"
#include "vm_stack.h"

/*
Here is defined a thunk interface between VM and native code.
Having this metadata simplifies things a lot, though the exact 
calling method is still platform dependent, as in C.
*/

#define VM_THUNK_ARGS_START \
	struct {

#define VM_THUNK_ARGS_END \
	} args; \
	vm_stack_pop3(ctx->dstack, (vm_operand_t *)&args, sizeof(args) / sizeof(vm_operand_t));

#define VM_THUNK_ARG(decl) union { decl __attribute__ ((aligned (sizeof(vm_operand_t)))) ; }

#define VM_THUNK_RETURN(what) \
	vm_stack_push(ctx->dstack, (vm_operand_t)what)

typedef void (*vm_thunk_t)(vm_context_t *ctx);

typedef struct {
	uint32_t hash;
	vm_thunk_t proc;
} vm_thunk_record_t;

#define VM_THUNK(name) \
	void __thunkproc_##name(vm_context_t *ctx)

#define VM_THUNK_DEFINE(name, hash) \
	vm_thunk_record_t __thunkrec_##name __attribute__ ((section (".thunks"))) = { hash, __thunkproc_##name }

extern vm_thunk_t vm_lookup_thunk(uint32_t hash);

#endif // __mette_vm_thunks_h_included

