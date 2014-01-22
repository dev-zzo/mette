#include "vm_internal.h"
#include "rtl_print.h"
#include "syscalls.h"

void __attribute__((noreturn)) vm_panic(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	rtl_vprint_fd(2, format, args);
	va_end(args);
	sys_exit(1);
	__builtin_unreachable();
}

