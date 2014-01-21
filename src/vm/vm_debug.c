#include "vm_internal.h"
#include "xstring.h"
#include "syscalls.h"
#include <stdlib.h>
#include <stdarg.h>

void __attribute__((noreturn)) vm_panic(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	sys_write(2, format, xstrlen(format));
	va_end(args);
	sys_exit(1);
}

