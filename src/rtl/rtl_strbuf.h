#ifndef __mette_rtl_strbuf_included
#define __mette_rtl_strbuf_included

#include <stdint.h>

typedef struct _rtl_strbuf rtl_strbuf_t;
struct _rtl_strbuf {
	uint16_t length;
	uint16_t max_length;
};

#define STRBUF_DATA(sb) ((char *)(sb) + sizeof(rtl_strbuf_t))

extern rtl_strbuf_t *
rtl_strbuf_alloc(uint16_t size);

extern void 
rtl_strbuf_free(rtl_strbuf_t *sb);

static inline uint16_t 
rtl_strbuf_get_length(const rtl_strbuf_t *sb)
{
	return sb->length;
}

static inline uint16_t 
rtl_strbuf_get_size(const rtl_strbuf_t *sb)
{
	return sb->max_length;
}

static inline void *
rtl_strbuf_get_buffer(const rtl_strbuf_t *sb)
{
	return STRBUF_DATA(sb);
}

extern void 
rtl_strbuf_set_length(rtl_strbuf_t *sb, uint16_t length);

extern rtl_strbuf_t *
rtl_strbuf_resize(rtl_strbuf_t *sb, uint16_t size);

extern void
rtl_strbuf_append(rtl_strbuf_t *sb, const rtl_strbuf_t *src);

extern void
rtl_strbuf_append_char(rtl_strbuf_t *sb, char ch);

extern void
rtl_strbuf_append_asciiz(rtl_strbuf_t *sb, const char *asciiz);

#endif // __mette_rtl_strbuf_included
