#include "rtl_strbuf.h"
#include "xstring.h"
#include "rtl_memory.h"
#include "vm_thunks.h"

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

rtl_strbuf_t *rtl_strbuf_alloc(uint16_t size)
{
	rtl_strbuf_t *sb = NULL;

	sb = (rtl_strbuf_t *)rtl_alloc(sizeof(rtl_strbuf_t) + size);
	sb->length = 0;
	sb->max_length = (uint16_t)size;

	return sb;
}

void rtl_strbuf_free(rtl_strbuf_t *sb)
{
	rtl_free(sb);
}

void rtl_strbuf_set_length(rtl_strbuf_t *sb, uint16_t length)
{
	sb->length = min(sb->max_length, length);
}

rtl_strbuf_t *rtl_strbuf_resize(rtl_strbuf_t *sb, uint16_t size)
{
	if (size > sb->max_length) {
		sb = (rtl_strbuf_t *)rtl_realloc(sb, sizeof(rtl_strbuf_t) + size);
		if (sb->length > size) {
			sb->length = size;
		}
		sb->max_length = size;
	}
	return sb;
}

static void __strbuf_append(rtl_strbuf_t *sb, void *src, uint16_t count)
{
	uint16_t dst_length;
	uint16_t to_copy;

	to_copy = min(count, sb->max_length - sb->length);
	xmemcpy(rtl_strbuf_get_buffer(sb) + sb->length, src, to_copy);
	sb->length += to_copy;
}

void rtl_strbuf_append(rtl_strbuf_t *sb, const rtl_strbuf_t *src)
{
	__strbuf_append(sb, rtl_strbuf_get_buffer(src), rtl_strbuf_get_length(src));
}

void rtl_strbuf_append_char(rtl_strbuf_t *sb, char ch)
{
	char tmp = ch;
	__strbuf_append(sb, &tmp, 1);
}

void rtl_strbuf_append_asciiz(rtl_strbuf_t *sb, const char *asciiz)
{
	__strbuf_append(sb, asciiz, xstrlen(asciiz));
}

VM_THUNK(rtl_strbuf_alloc)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(uint16_t size);
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
		VM_THUNK_ARG(uint16_t new_length);
	VM_THUNK_ARGS_END

	rtl_strbuf_set_length(args.sb, args.new_length);
}

VM_THUNK(rtl_strbuf_resize)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(rtl_strbuf_t *sb);
		VM_THUNK_ARG(uint16_t new_size);
	VM_THUNK_ARGS_END

	VM_THUNK_RETURN(rtl_strbuf_resize(args.sb, args.new_size));
}

