#include "rtl_strbuf.h"
#include <stdarg.h>

static unsigned __asciiz2uint(const char *ptr);

typedef struct ___print_context_t __print_context_t;

typedef int (*__put_proc_t)(rtl_print_context_t *pctx, char ch);

struct ___print_context_t {
	const char *format;
	rtl_strbuf *dest_sb; /* Where we are writing to */

	union {
		struct {
			unsigned zero_padded : 1;
			unsigned left_adjusted : 1;
			unsigned width_given : 1;
			unsigned precision_given : 1;
		};
		unsigned all;
	} flags;
	unsigned width;
	unsigned precision;

	union {
		va_list varargs;
	} reader_data;

	__put_proc_t put_proc;
};

int rtl_print_str(rtl_strbuf *sb, const char *format, ...)
{
	__print_context_t pctx;

	pctx.format = format;
	pctx.dest_sb = sb;
	va_start(pctx.reader_data.varargs, format);

	va_end(pctx.reader_data.varargs);

	return rtl_strbuf_get_length(sb);
}

int rtl_print_fd(int fd, const char *format, ...)
{
	/* In unix, we are allowed to sys_write() to socket FDs. */
}

static int xisdigit(char ch)
{
	return ch >= '0' && ch <= '9';
}

static void rtl_print_internal(__print_context_t *pctx)
{
	const char *cursor = pctx->format;
	const char *escape_start;
	int escaped = 0;
	int state;

	while (*cursor) {

		if (!escaped) {
			if (*cursor == '%') {
				escaped = 1;
				state = 0;
			} else {
				pctx->put_proc(pctx, *cursor);
			}
			++cursor;
			continue;
		}

		if (*cursor == '%') {
			pctx->put_proc(pctx, *cursor);
			escaped = 0;
			++cursor;
			continue;
		}
		if (*cursor == '0') {
			/* Zero padding flag */
			pctx->flags.zero_padded = 1;
			++cursor;
			continue;
		}
		if (*cursor == '-') {
			/* Left alignment flag */
			pctx->flags.left_aligned = 1;
			pctx->flags.zero_padded = 0;
			++cursor;
			continue;
		}
		if (xisdigit(*cursor)) {
			/* Field width specifier */
			pctx->flags.width_given = 1;
			while (xisdigit(*cursor)) {
				pctx->width = pctx->width * 10 + *cursor - '0';
				++cursor;
			}
			/* End of format? */
			if (*cursor == '\0')
				break;
		}
		if (*cursor == '.') {
			/* Precision separator */
			++cursor;
			/* End of format? */
			if (*cursor == '\0')
				break;
			/* Precision field */
			pctx->flags.prec_given = 1;
			while (xisdigit(*cursor)) {
				pctx->prec = pctx->prec * 10 + *cursor - '0';
				++cursor;
			}
			/* End of format? */
			if (*cursor == '\0')
				break;
		}
		switch (*cursor) {
			case 'd':
				/* Signed decimal */
				break;
			case 'u':
				/* Unsigned decimal */
				break;
			case 'o':
				/* Unsigned octal */
				break;
			case 'x':
				/* Unsigned hex, upper-case */
				break;
			case 'c':
				/* A character */
				break;
			case 's':
				/* A strbuf */
				break;
			case 'a':
				/* An ASCIIZ string */
				break;
			default:
				/* Incorrect specifier */
				break;						
		}
		escaped = 0;
		++cursor;
	}

end:
}

