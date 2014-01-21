#include "syscalls.h"
#include "rtl_memory.h"
#include "vm_thunks.h"
#include <sys/select.h>

VM_THUNK(fdset_create)
{
	fd_set *fds;

	fds = (fd_set *)rtl_alloc(sizeof(*fds));
	FD_ZERO(fds);

	VM_THUNK_RETURN(fds);
}

/* fdset can be freed with rtl_free. No separate method. */

VM_THUNK(fdset_set)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(fd_set *fds);
		VM_THUNK_ARG(int fd);
	VM_THUNK_ARGS_END

	FD_SET(args.fd, args.fds);
}

VM_THUNK(fdset_clear)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(fd_set *fds);
		VM_THUNK_ARG(int fd);
	VM_THUNK_ARGS_END

	FD_CLR(args.fd, args.fds);
}

VM_THUNK(fdset_check)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(fd_set *fds);
		VM_THUNK_ARG(int fd);
	VM_THUNK_ARGS_END

	VM_THUNK_RETURN(FD_ISSET(args.fd, args.fds));
}

VM_THUNK(io_select)
{
	int result;
	struct timeval tv;

	VM_THUNK_ARGS_START
		VM_THUNK_ARG(fd_set *rfds);
		VM_THUNK_ARG(fd_set *wfds);
		VM_THUNK_ARG(fd_set *efds);
		VM_THUNK_ARG(int timeout);
	VM_THUNK_ARGS_END

	tv.tv_sec = args.timeout / 1000;
	tv.tv_usec = args.timeout * 1000;
	result = sys__newselect(
		1024,
		args.rfds,
		args.wfds,
		args.efds,
		&tv);

	VM_THUNK_RETURN(result);
}
