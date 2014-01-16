#ifndef __mette_sysdep_xstring_h_included
#define __mette_sysdep_xstring_h_included

#include <stddef.h>

extern void *xmemcpy(void *dest, const void *src, size_t count);

extern void *xmemset(void *dest, int c, size_t count);

extern size_t xstrlen(const char *str);

#endif // __mette_sysdep_xstring_h_included

