#ifndef __mette_vm_thunks_h_included
#define __mette_vm_thunks_h_included

#include <stdint.h>

/*

Here is defined a thunk interface between VM and native code.
Having this metadata simplifies things a lot, though the exact 
calling method is still platform dependent, as in C.

Calling vararg functions seems to be out of question right now.

*/

#define VM_MAX_ARGS 7

typedef vm_operand_t (*vm_thunk_rv)();
typedef void (*vm_thunk_norv)();

typedef struct {
	uint32_t hash;
	void *target;
	uint32_t argc : 3;
	uint32_t has_rv : 1;
	/* spare bits */
} vm_thunk_t;

extern const vm_thunk_t *vm_find_thunk(uint32_t hash);

#endif // __mette_vm_thunks_h_included

