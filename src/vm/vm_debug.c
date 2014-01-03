#include "vm_internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void __attribute__((noreturn)) vm_panic(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	write(2, format, strlen(format));
	va_end(args);
	exit(1);
}

