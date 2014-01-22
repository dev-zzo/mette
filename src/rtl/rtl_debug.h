#ifndef __mette_rtl_debug_included
#define __mette_rtl_debug_included

#ifdef DEBUG_PRINTS
#include "rtl_print.h"
#define DBGPRINT(...) rtl_print_fd(2, __VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#endif // __mette_rtl_debug_included
