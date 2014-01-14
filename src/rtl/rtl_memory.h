#ifndef __mette_rtl_memory_included
#define __mette_rtl_memory_included

#include <stddef.h>

extern void *rtl_alloc(size_t size);

extern void *rtl_realloc(void *ptr, size_t size);

extern void rtl_free(void *ptr);

#endif // __mette_rtl_memory_included
