#include "vm_thunks.h"
#include "rtl_memory.h"
#include "rtl_strbuf.h"
#include "rtl_print.h"
#include "xstring.h"

/* rtl_memory */

VM_THUNK(rtl_alloc)
{
	void *ptr;
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(size_t size);
	VM_THUNK_ARGS_END
	ptr = rtl_alloc(args.size);
	VM_THUNK_RETURN(ptr);
}

VM_THUNK(rtl_realloc)
{
	void *ptr;
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(void *ptr);
		VM_THUNK_ARG(size_t size);
	VM_THUNK_ARGS_END
	ptr = rtl_realloc(args.ptr, args.size);
	VM_THUNK_RETURN(ptr);
}

VM_THUNK(rtl_free)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(void *ptr);
	VM_THUNK_ARGS_END
	rtl_free(args.ptr);
}

VM_THUNK(rtl_memset)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(void *dest);
		VM_THUNK_ARG(int value);
		VM_THUNK_ARG(size_t size);
	VM_THUNK_ARGS_END

	xmemset(args.dest, args.value, args.size);
}

VM_THUNK(rtl_memcpy)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(void *dest);
		VM_THUNK_ARG(const void *src);
		VM_THUNK_ARG(size_t size);
	VM_THUNK_ARGS_END

	xmemcpy(args.dest, args.src, args.size);
}

/* rtl_strbuf */

VM_THUNK(rtl_strbuf_alloc)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(size_t size);
	VM_THUNK_ARGS_END

	VM_THUNK_RETURN(rtl_strbuf_alloc(args.size));
}

VM_THUNK(rtl_strbuf_free)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(rtl_strbuf_t *sb);
	VM_THUNK_ARGS_END

	rtl_strbuf_free(args.sb);	
}

VM_THUNK(rtl_strbuf_get_length)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(rtl_strbuf_t *sb);
	VM_THUNK_ARGS_END

	VM_THUNK_RETURN(rtl_strbuf_get_length(args.sb));
}

VM_THUNK(rtl_strbuf_get_size)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(rtl_strbuf_t *sb);
	VM_THUNK_ARGS_END

	VM_THUNK_RETURN(rtl_strbuf_get_size(args.sb));
}

VM_THUNK(rtl_strbuf_get_buffer)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(rtl_strbuf_t *sb);
	VM_THUNK_ARGS_END

	VM_THUNK_RETURN(rtl_strbuf_get_buffer(args.sb));
}

VM_THUNK(rtl_strbuf_set_length)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(rtl_strbuf_t *sb);
		VM_THUNK_ARG(size_t new_length);
	VM_THUNK_ARGS_END

	rtl_strbuf_set_length(args.sb, args.new_length);
}

VM_THUNK(rtl_strbuf_resize)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(rtl_strbuf_t *sb);
		VM_THUNK_ARG(size_t new_size);
	VM_THUNK_ARGS_END

	VM_THUNK_RETURN(rtl_strbuf_resize(args.sb, args.new_size));
}

/* rtl_print */

static uintptr_t nextarg_stack(void *context)
{
	uintptr_t value;
	vm_context_t *ctx = (vm_context_t *)context;
	vm_stack_pop3(ctx->dstack, &value, 1);
	return value;
}

VM_THUNK(rtl_print_fd)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int fd);
		VM_THUNK_ARG(const char *format);
	VM_THUNK_ARGS_END

	VM_THUNK_RETURN(rtl_xprint_fd(args.fd, args.format, nextarg_stack, ctx));
}

VM_THUNK(rtl_print_sb)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(rtl_strbuf_t *sb);
		VM_THUNK_ARG(const char *format);
	VM_THUNK_ARGS_END

	VM_THUNK_RETURN(rtl_xprint_sb(args.sb, args.format, nextarg_stack, ctx));
}

