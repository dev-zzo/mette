#include "vm_internal.h"
#include "vm_thunks.h"

/* Linker defined */
extern vm_thunk_record_t __vm_thunks_start;
extern vm_thunk_record_t __vm_thunks_end;

vm_thunk_t vm_lookup_thunk(uint32_t hash)
{
	vm_thunk_record_t *thunk = &__vm_thunks_start;
	while (thunk != &__vm_thunks_end) {
		if (thunk->hash == hash) {
			return thunk->proc;
		}
	}
	return (vm_thunk_t)0;
}

