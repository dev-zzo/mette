#include "vm_internal.h"
#include "rtl_print.h"
#include "syscalls.h"

void __attribute__((noreturn)) vm_panic(const char *format, ...)
{
	va_list args;
	
	rtl_print_fd(2, "vm_panic: ");
	va_start(args, format);
	rtl_vprint_fd(2, format, args);
	va_end(args);
	rtl_print_fd(2, "\n\n");
	sys_exit(1);
	__builtin_unreachable();
}

