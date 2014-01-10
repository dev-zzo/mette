#include "vm_thunks.h"
#include "xmalloc.h"

VM_THUNK(rtl_alloc)
{
	void *ptr;
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(size_t size);
	VM_THUNK_ARGS_END
	ptr = xmalloc(args.size);
	VM_THUNK_RETURN(ptr);
}

VM_THUNK(rtl_realloc)
{
	void *ptr;
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(void * ptr);
		VM_THUNK_ARG(size_t size);
	VM_THUNK_ARGS_END
	ptr = xrealloc(args.ptr, args.size);
	VM_THUNK_RETURN(ptr);
}

VM_THUNK(rtl_free)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(void *ptr);
	VM_THUNK_ARGS_END
	xfree(args.ptr);
}
