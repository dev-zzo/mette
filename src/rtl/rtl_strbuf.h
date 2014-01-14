#ifndef __mette_rtl_strbuf_included
#define __mette_rtl_strbuf_included

#include <stdint.h>

typedef struct _rtl_strbuf rtl_strbuf_t;

extern rtl_strbuf_t *
strbuf_alloc(uint16_t size);

extern void 
strbuf_free(rtl_strbuf_t *sb);

extern uint16_t 
strbuf_get_length(rtl_strbuf_t *sb);

extern uint16_t 
strbuf_get_size(rtl_strbuf_t *sb);

extern void *
strbuf_get_buffer(rtl_strbuf_t *sb);

extern void 
strbuf_set_length(rtl_strbuf_t *sb, uint16_t length);

extern rtl_strbuf_t *
strbuf_resize(rtl_strbuf_t *sb, uint16_t size);

#endif // __mette_rtl_strbuf_included
