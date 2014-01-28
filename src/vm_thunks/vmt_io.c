#include "syscalls.h"
#include "vm_thunks.h"
#include "rtl_strbuf.h"

VM_THUNK(io_open)
{
	int fd;
	const char *path;
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(rtl_strbuf_t *sb_path);
		VM_THUNK_ARG(int flags);
		VM_THUNK_ARG(int mode);
	VM_THUNK_ARGS_END

	fd = sys_open(path, args.flags, args.mode);

	VM_THUNK_RETURN(fd);
}

VM_THUNK(io_close)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int fd);
	VM_THUNK_ARGS_END

	sys_close(args.fd);
}


