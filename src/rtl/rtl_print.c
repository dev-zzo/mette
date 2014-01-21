#include "rtl_print.h"
#include "rtl_strbuf.h"
#include "xstring.h"
#include "syscalls.h"
#include <stdarg.h>
#include <stdint.h>

//#define DBGPRINT

typedef struct ___print_context_t __print_context_t;

typedef void (*__flush_proc_t)(__print_context_t *pctx);

#define BUFFER_SIZE 1024

struct ___print_context_t {
	/* Settings for current conversion */
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

	/* Arg reader context */
	rtl_print_nextarg_proc_t nextarg_proc;
	void *reader_data;

	/* Output writer context */
	char buffer[BUFFER_SIZE];
	unsigned buffer_mark;
	int write_count;
	__flush_proc_t flush_proc;
	union {
		rtl_strbuf_t *sb;
		int fd;
	} writer_data;
};

static int xisdigit(char ch)
{
	return ch >= '0' && ch <= '9';
}

static void put_char(__print_context_t *pctx, char ch)
{
	unsigned buffer_mark = pctx->buffer_mark;

	pctx->buffer[buffer_mark++] = ch;

	if (buffer_mark == BUFFER_SIZE) {
		pctx->flush_proc(pctx);
		pctx->write_count += buffer_mark;
		buffer_mark = 0;
	}

	pctx->buffer_mark = buffer_mark;
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
			put_char(pctx, *text);
			text++;
			text_length--;
		}
		while (space_count) {
			put_char(pctx, ' ');
			space_count--;
		}
	} else {
		while (space_count) {
			put_char(pctx, pad);
			space_count--;
		}
		while (text_length) {
			put_char(pctx, *text);
			text++;
			text_length--;
		}
	}
}

static const char conv_digits[] = "0123456789ABCDEFHGIJKLMNOPQRSTUVWXYZ";

static char *conv_base_r(char *buffer, unsigned base, unsigned value)
{
	unsigned next = value / base;
	unsigned rem = value % base;

	if (next) {
		buffer = conv_base_r(buffer, base, next);
	}

	*buffer = conv_digits[rem];
	return buffer + 1;
}

static size_t conv_base(char *buffer, unsigned base, unsigned value)
{
	return conv_base_r(buffer, base, value) - buffer;
}

static void conv_d(__print_context_t *pctx, int value)
{
	char digits[34];
	size_t digit_count;

#ifdef DBGPRINT
	xmemset(digits, 'X', 33);
	digits[33] = '\n';
#endif

	if (value < 0) {
		digit_count = conv_base(digits + 1, 10, -value) + 1;
		digits[0] = '-';
		conv_text(pctx, digits, digit_count, ' ');
	} else {
		digit_count = conv_base(digits, 10, value);
		conv_text(pctx, digits, digit_count, pctx->flags.zero_padded ? '0' : ' ');
	}
#ifdef DBGPRINT
	sys_write(2, digits, 34);
#endif
}

static void conv_uox(__print_context_t *pctx, unsigned base, unsigned value)
{
	char digits[33];
	size_t digit_count;

#ifdef DBGPRINT
	xmemset(digits, 'X', 32);
	digits[32] = '\n';
#endif

	digit_count = conv_base(digits, base, value);

	conv_text(pctx, digits, digit_count, pctx->flags.zero_padded ? '0' : ' ');

#ifdef DBGPRINT
	sys_write(2, digits, 33);
#endif
}

static void conv_strbuf(__print_context_t *pctx, const rtl_strbuf_t *sb)
{
	conv_text(pctx, rtl_strbuf_get_buffer(sb), rtl_strbuf_get_length(sb), ' ');
}

static void conv_asciiz(__print_context_t *pctx, const char *text)
{
	conv_text(pctx, text, xstrlen(text), ' ');
}

static int __ascii2uint(const char *text, unsigned *result)
{
	unsigned value = 0;
	const char *cursor = text;

	while (xisdigit(*cursor)) {
		value = value * 10 + *cursor - '0';
		++cursor;
	}

	*result = value;
	return cursor - text;
}

static void __print_internal(const char *format, __print_context_t *pctx)
{
	const char *cursor = format;
	int done = 0;

	do {

		while (*cursor) {
			if (*cursor != '%') {
				put_char(pctx, *cursor);
				++cursor;
			} else {
				++cursor;
				pctx->flags.all = 0;
				break;
			}
		}

		if (*cursor == '%') {
			put_char(pctx, *cursor);
			++cursor;
			continue;
		}

		if (*cursor == '0') {
			/* Zero padding flag */
			pctx->flags.zero_padded = 1;
			++cursor;
#ifdef DBGPRINT
			sys_write(2, "Z", 1);
#endif
		}
		if (*cursor == '-') {
			/* Left alignment flag */
			pctx->flags.left_adjusted = 1;
			++cursor;
#ifdef DBGPRINT
			sys_write(2, "L", 2);
#endif
		}
		if (xisdigit(*cursor)) {
			/* Field width specifier */
			pctx->flags.width_given = 1;
			cursor += __ascii2uint(cursor, &pctx->width);
		}
		if (*cursor == '.') {
			/* Precision separator */
			++cursor;
			/* End of format check not needed. */
			/* Precision field */
			pctx->flags.prec_given = 1;
			cursor += __ascii2uint(cursor, &pctx->prec);
		}
		switch (*cursor) {
			case 'd':
				/* Signed decimal */
				conv_d(pctx, (int)pctx->nextarg_proc(pctx));
				break;
			case 'u':
				/* Unsigned decimal */
				conv_uox(pctx, 10, (unsigned)pctx->nextarg_proc(pctx));
				break;
			case 'o':
				/* Unsigned octal */
				conv_uox(pctx, 18, (unsigned)pctx->nextarg_proc(pctx));
				break;
			case 'x':
			case 'X':
				/* Unsigned hex, upper-case */
				conv_uox(pctx, 16, (unsigned)pctx->nextarg_proc(pctx));
				break;
			case 'c':
				/* A character */
				put_char(pctx, (char)pctx->nextarg_proc(pctx));
				break;
			case 's':
				/* A strbuf */
				conv_strbuf(pctx, (const rtl_strbuf_t *)pctx->nextarg_proc(pctx));
				break;
			case 'z':
				/* An ASCIIZ string */
				conv_asciiz(pctx, (const char *)pctx->nextarg_proc(pctx));
				break;
			case '\0':
				done = 1;
				break;
			default:
				/* Incorrect specifier */
				break;						
		}
		++cursor;
	} while (!done);

	if (pctx->buffer_mark) {
		pctx->flush_proc(pctx);
		pctx->write_count += pctx->buffer_mark;
		pctx->buffer_mark = 0;
	}
}

static uintptr_t nextarg_va_list(void *context)
{
	return va_arg(*(va_list *)context, uintptr_t);
}

static void flush_sb(__print_context_t *pctx)
{
	rtl_strbuf_append_bytes(pctx->writer_data.sb, pctx->buffer, pctx->buffer_mark);
}

static void flush_fd(__print_context_t *pctx)
{
	sys_write(pctx->writer_data.fd, pctx->buffer, pctx->buffer_mark);
}

int rtl_print_sb4(rtl_strbuf_t *sb, const char *format, rtl_print_nextarg_proc_t nextarg_proc, void *context)
{
	__print_context_t pctx;

	pctx.nextarg_proc = nextarg_proc;
	pctx.reader_data = context;
	pctx.flush_proc = flush_sb;
	pctx.writer_data.sb = sb;
	pctx.buffer_mark = 0;
	pctx.write_count = 0;

	__print_internal(format, &pctx);

	return rtl_strbuf_get_length(sb);
}

int rtl_print_sb(rtl_strbuf_t *sb, const char *format, ...)
{
	va_list args;
	int result;

	va_start(args, format);

	result = rtl_print_sb4(sb, format, nextarg_va_list, &args);

	va_end(args);

	return result;
}

int rtl_print_fd4(int fd, const char *format, rtl_print_nextarg_proc_t nextarg_proc, void *context)
{
	__print_context_t pctx;

	pctx.nextarg_proc = nextarg_proc;
	pctx.reader_data = context;
	pctx.flush_proc = flush_fd;
	pctx.writer_data.fd = fd;
	pctx.buffer_mark = 0;
	pctx.write_count = 0;

	__print_internal(format, &pctx);

	return pctx.write_count;
}

int rtl_print_fd(int fd, const char *format, ...)
{
	va_list args;
	int result;

	va_start(args, format);

	result = rtl_print_fd4(fd, format, nextarg_va_list, &args);

	va_end(args);

	return result;
}

