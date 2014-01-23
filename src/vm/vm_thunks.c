#include "vm_internal.h"
#include "vm_thunks.h"

/*
#define DEBUG_PRINTS
*/
#include "rtl_debug.h"

/* Linker defined */
extern const vm_thunk_record_t vm_thunks[];
extern vm_thunk_record_t vm_thunks_end;

vm_thunk_t vm_lookup_thunk(uint32_t hash)
{
	const vm_thunk_record_t *thunk = vm_thunks;

	DBGPRINT("vm_lookup_thunk: searching for %08x.\n", hash);
	while (thunk != &vm_thunks_end) {
		if (thunk->hash == hash) {
			return thunk->proc;
		}
		thunk++;
	}
	DBGPRINT("vm_lookup_thunk: not found.\n");
	return (vm_thunk_t)0;
}

