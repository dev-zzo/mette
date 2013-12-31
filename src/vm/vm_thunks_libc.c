#include "vm_thunks.h"
#include "xmalloc.h"

VM_THUNK(xmalloc, 0xC3150001)
{
	void *ptr;
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(size_t size);
	VM_THUNK_ARGS_END
	ptr = xmalloc(args.size);
	VM_THUNK_RETURN(ptr);
}

VM_THUNK(xrealloc, 0xC3150002)
{
	void *ptr;
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(void * ptr);
		VM_THUNK_ARG(size_t size);
	VM_THUNK_ARGS_END
	ptr = xrealloc(args.ptr, args.size);
	VM_THUNK_RETURN(ptr);
}

VM_THUNK(xfree, 0xC3150003)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(void *ptr);
	VM_THUNK_ARGS_END
	xfree(args.ptr);
}


