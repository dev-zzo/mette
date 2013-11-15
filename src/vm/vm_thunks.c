#include "vm_internal.h"
#include "vm_thunks.h"

/*
Refer to thunks array defined somewhere else.
TODO: Make this a linker's job.
*/
extern vm_thunk_t vm_thunks[];

const vm_thunk_t *vm_find_thunk(uint32_t hash)
{
	return 0;
}

