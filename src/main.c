#include "vm.h"
#include "syscalls.h"
#include "rtl_print.h"
#include "rtl_memory.h"
#include "vm_thunks.h"

void rtl_alloc_dump(void);

int main(int argc, char *argv[])
{
	void *p1, *p2, *p3, *p4, *p5;

	vm_lookup_thunk(12234);

	rtl_print_fd(1, "\n\nDone.\n");
	sys_exit(24);
	return 0;
}
