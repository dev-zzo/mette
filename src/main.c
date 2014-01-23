#include "vm.h"
#include "syscalls.h"
#include "rtl_print.h"
#include "rtl_memory.h"
#include "vm_thunks.h"
#include "vm_loader.h"

int main(int argc, char *argv[])
{
	int fd;
	vm_module_t *module;
	vm_context_t *context;

	argv++; argc--;

	if (argc == 0) {
		rtl_print_fd(2, "main: no file name provided.\n");
		return 1;
	}

	rtl_print_fd(2, "main: reading '%s'\n", argv[0]);
	
	fd = vm_open(argv[0], O_RDONLY, 0);
	if (fd < 0) {
		vm_panic("vm_load_exec: failed to open the image.");
	}
	
	module = vm_load_fd(fd);
	vm_close(fd);
	if (!module) {
		vm_panic("vm_load_exec: failed to load the image.");
	}

	context = vm_context_create(module);
	
	for (;;) {
		vm_step(context);
	}
	
	rtl_print_fd(1, "\n\nDone.\n");
	return 0;
}
