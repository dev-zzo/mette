#ifndef __mette_sysdep_xstddef_h_included
#define __mette_sysdep_xstddef_h_included

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef _GCC_SIZE_T
typedef unsigned long size_t;
typedef long ssize_t;
#endif

#ifndef offsetof
#define offsetof(TYPE, MEMBER) __builtin_offsetof (TYPE, MEMBER)
#endif

#endif // __mette_sysdep_xstddef_h_included

