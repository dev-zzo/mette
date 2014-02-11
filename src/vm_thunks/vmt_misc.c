#include "syscalls.h"
#include "vm_thunks.h"

VM_THUNK(sys_exit)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int code);
	VM_THUNK_ARGS_END
	sys_exit(args.code);
}

VM_THUNK(sys_time)
{
	time_t tv;

	sys_time(&tv);

	VM_THUNK_RETURN(tv);
}
