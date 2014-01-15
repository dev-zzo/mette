#include "rtl_strbuf.h"
#include <stdarg.h>

typedef struct _rtl_print_context_t rtl_print_context_t;
struct _rtl_print_context_t {
	rtl_strbuf *dest_sb; /* Where we are writing to */

	union {
		va_list varargs;
	} reader_data;
};

typedef int (rtl_print_proc_t)();

int rtl_print_str(rtl_strbuf *sb, const char *format, ...)
{
	rtl_print_context_t pctx;

	pctx.dest_sb = sb;
	va_start(pctx.reader_data.varargs, format);

	va_end(pctx.reader_data.varargs);

	return rtl_strbuf_get_length(sb);
}

int rtl_print_fd(int fd, const char *format, ...)
{
	/* In unix, we are allowed to sys_write() to socket FDs. */
}

