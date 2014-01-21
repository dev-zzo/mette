#include "xstring.h"
#include "rtl_strbuf.h"
#include "rtl_memory.h"

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

void rtl_strbuf_append_bytes(rtl_strbuf_t *sb, const void *src, uint16_t count)
{
	uint16_t dst_length;
	uint16_t to_copy;

	to_copy = min(count, sb->max_length - sb->length);
	xmemcpy(rtl_strbuf_get_buffer(sb) + sb->length, src, to_copy);
	sb->length += to_copy;
}

void rtl_strbuf_append(rtl_strbuf_t *sb, const rtl_strbuf_t *src)
{
	rtl_strbuf_append_bytes(sb, rtl_strbuf_get_buffer(src), rtl_strbuf_get_length(src));
}

void rtl_strbuf_append_char(rtl_strbuf_t *sb, char ch)
{
	char tmp = ch;
	rtl_strbuf_append_bytes(sb, &tmp, 1);
}

void rtl_strbuf_append_asciiz(rtl_strbuf_t *sb, const char *asciiz)
{
	rtl_strbuf_append_bytes(sb, asciiz, xstrlen(asciiz));
}
