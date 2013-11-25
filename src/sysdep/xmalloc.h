#ifndef __mette_sysdep_xmalloc_h_included
#define __mette_sysdep_xmalloc_h_included

#include "xstddef.h"

extern void *xmalloc(size_t size);

extern void *xrealloc(void *ptr, size_t size);

extern void xfree(void *ptr);

#endif // __mette_sysdep_xmalloc_h_included

