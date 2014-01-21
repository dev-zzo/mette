#ifndef __mette_rtl_print_included
#define __mette_rtl_print_included

#include "rtl_strbuf.h"

extern int rtl_print_sb(rtl_strbuf_t *sb, const char *format, ...);
extern int rtl_print_fd(int fd, const char *format, ...);

#endif // __mette_rtl_print_included
