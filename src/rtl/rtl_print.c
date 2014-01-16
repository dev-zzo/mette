#include "rtl_strbuf.h"
#include "xstring.h"
#include <stdarg.h>
#include <stdint.h>

static unsigned __asciiz2uint(const char *ptr);

typedef struct ___print_context_t __print_context_t;

typedef int (*__put_proc_t)(__print_context_t *pctx, char ch);
typedef uintptr_t (*__nextarg_proc_t)(__print_context_t *pctx);

struct ___print_context_t {
	const char *format;
	rtl_strbuf_t *dest_sb; /* Where we are writing to */

	union {
		struct {
			unsigned zero_padded : 1;
			unsigned left_adjusted : 1;
			unsigned width_given : 1;
			unsigned prec_given : 1;
		};
		unsigned all;
	} flags;
	unsigned width;
	unsigned prec;

	union {
		va_list varargs;
	} reader_data;

	__put_proc_t put_proc;
	__nextarg_proc_t nextarg_proc;
};

static uintptr_t nextarg_va_list(__print_context_t *pctx)
{
	return va_arg(pctx->reader_data.varargs, uintptr_t);
}

int rtl_print_str(rtl_strbuf_t *sb, const char *format, ...)
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

static void conv_text(__print_context_t *pctx, const char *text, size_t text_length, char pad)
{
	size_t space_count = 0;

	if (pctx->flags.prec_given && pctx->prec < text_length) {
		text_length = pctx->prec;
	}

	if (pctx->flags.width_given && pctx->width > text_length) {
		space_count = pctx->width - text_length;
	}

	if (pctx->flags.left_adjusted) {
		while (text_length) {
			pctx->put_proc(pctx, *text);
			text++;
			text_length--;
		}
		while (space_count) {
			pctx->put_proc(pctx, ' ');
			space_count--;
		}
	} else {
		while (space_count) {
			pctx->put_proc(pctx, pad);
			space_count--;
		}
		while (text_length) {
			pctx->put_proc(pctx, *text);
			text++;
			text_length--;
		}
	}
}

static const char conv_digits[] = "0123456789ABCDEFHGIJKLMNOPQRSTUVWXYZ";

char *conv_base_r(char *buffer, int base, unsigned value)
{
	unsigned next = value / base;
	unsigned rem = value % base;

	if (next) {
		buffer = conv_base_r(buffer, base, value);
	}

	*buffer = conv_digits[rem];
	return buffer + 1;
}

static size_t conv_base(char *buffer, int base, unsigned value)
{
	return conv_base_r(buffer, base, value) - buffer;
}

static void conv_d(__print_context_t *pctx, int value)
{
	char digits[33];
	size_t digit_count;

	if (value < 0) {
		digit_count = conv_base(digits + 1, 10, -value) + 1;
		digits[0] = '-';
		conv_text(pctx, digits, digit_count, ' ');
	} else {
		digit_count = conv_base(digits, 10, value);
		conv_text(pctx, digits, digit_count, pctx->flags.zero_padded ? '0' : ' ');
	}
}

static void conv_u(__print_context_t *pctx, unsigned value)
{
	char digits[32];
	size_t digit_count;

	digit_count = conv_base(digits, 10, value);

	conv_text(pctx, digits, digit_count, pctx->flags.zero_padded ? '0' : ' ');
}

static void conv_strbuf(__print_context_t *pctx, const rtl_strbuf_t *sb)
{
	conv_text(pctx, rtl_strbuf_get_buffer(sb), rtl_strbuf_get_length(sb), ' ');
}

static void conv_asciiz(__print_context_t *pctx, const char *text)
{
	conv_text(pctx, text, xstrlen(text), ' ');
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
				pctx->flags.all = 0;
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
			pctx->flags.left_adjusted = 1;
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
				pctx->put_proc(pctx, (char)pctx->nextarg_proc(pctx));
				break;
			case 's':
				/* A strbuf */
				conv_strbuf(pctx, (const rtl_strbuf_t *)pctx->nextarg_proc(pctx));
				break;
			case 'z':
				/* An ASCIIZ string */
				conv_asciiz(pctx, (const char *)pctx->nextarg_proc(pctx));
				break;
			default:
				/* Incorrect specifier */
				break;						
		}
		escaped = 0;
		++cursor;
	}

}

