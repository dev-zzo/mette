#ifndef __mette_sysdep_xstring_h_included
#define __mette_sysdep_xstring_h_included

#include "xstddef.h"

extern void *xmemcpy(void *dest, const void *src, size_t count);

extern void *xmemset(void *dest, int c, size_t count);

#endif // __mette_sysdep_xstring_h_included

