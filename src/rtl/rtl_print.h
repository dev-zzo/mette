#ifndef __mette_rtl_print_included
#define __mette_rtl_print_included

#include "rtl_strbuf.h"

typedef uintptr_t (*rtl_print_nextarg_proc_t)(void *context);

extern int rtl_print_sb(rtl_strbuf_t *sb, const char *format, ...);
extern int rtl_print_sb4(rtl_strbuf_t *sb, const char *format, rtl_print_nextarg_proc_t nextarg_proc, void *context);
extern int rtl_print_fd(int fd, const char *format, ...);
extern int rtl_print_fd4(int fd, const char *format, rtl_print_nextarg_proc_t nextarg_proc, void *context);

#endif // __mette_rtl_print_included
