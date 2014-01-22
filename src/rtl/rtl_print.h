#ifndef __mette_rtl_print_included
#define __mette_rtl_print_included

#include "rtl_strbuf.h"
#include <stdarg.h>

typedef uintptr_t (*rtl_print_nextarg_proc_t)(void *context);

extern int rtl_xprint_sb(rtl_strbuf_t *sb, const char *format, rtl_print_nextarg_proc_t nextarg_proc, void *context);
extern int rtl_print_sb(rtl_strbuf_t *sb, const char *format, ...);

extern int rtl_xprint_fd(int fd, const char *format, rtl_print_nextarg_proc_t nextarg_proc, void *context);
extern int rtl_vprint_fd(int fd, const char *format, va_list args);
extern int rtl_print_fd(int fd, const char *format, ...);

#endif // __mette_rtl_print_included
