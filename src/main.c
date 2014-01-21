#include "vm.h"
#include "syscalls.h"
#include "rtl_print.h"
#include "rtl_memory.h"

void rtl_alloc_dump(void);

int main(int argc, char *argv[])
{
	void *p;

	rtl_alloc_dump();

	p = rtl_alloc(66);
	rtl_alloc_dump();

	sys_exit(24);
	return 0;
}
