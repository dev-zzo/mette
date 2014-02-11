#include "syscalls.h"
#include "vm_thunks.h"
#include "rtl_strbuf.h"

VM_THUNK(io_open)
{
	int fd;
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(rtl_strbuf_t *sb_path);
		VM_THUNK_ARG(int flags);
		VM_THUNK_ARG(int mode);
	VM_THUNK_ARGS_END

	fd = sys_open(rtl_strbuf_to_asciiz(args.sb_path), args.flags, args.mode);

	VM_THUNK_RETURN(fd);
}

VM_THUNK(io_read)
{
	rtl_strbuf_t *sb;
	ssize_t rv;
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int fd);
		VM_THUNK_ARG(size_t count);
	VM_THUNK_ARGS_END

	sb = rtl_strbuf_alloc(args.count);
	rv = sys_read(args.fd, rtl_strbuf_get_buffer(sb), rtl_strbuf_get_size(sb));
	if (rv < 0) {
		rtl_strbuf_free(sb);
		VM_THUNK_RETURN(NULL);
	}
	rtl_strbuf_set_length(sb, rv);
	VM_THUNK_RETURN(sb);
}

VM_THUNK(io_write)
{
	ssize_t rv;
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int fd);
		VM_THUNK_ARG(rtl_strbuf_t *sb);
	VM_THUNK_ARGS_END

	rv = sys_write(args.fd, rtl_strbuf_get_buffer(args.sb), rtl_strbuf_get_length(args.sb));

	VM_THUNK_RETURN(rv);
}

VM_THUNK(io_close)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int fd);
	VM_THUNK_ARGS_END

	sys_close(args.fd);
}


