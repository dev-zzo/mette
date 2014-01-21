#include "vm_loader.h"
#include "vm_sysdeps.h"
#include "vm_internal.h"
#include <elf.h>

static void vm_resolve_ncalls()
{

}

vm_module_t *vm_load_fd(int fd)
{
	vm_module_t *module;
	char signature[4];
	unsigned module_memsize;
	unsigned module_filesize;

	if (vm_read(fd, signature, 4) != 4) {
		vm_panic("vm_load_fd: input too short.");
	}
	if (signature[0] != 'B' || signature[1] != 'A' || signature[2] != 'R' || signature[3] != 'F') {
		vm_panic("vm_load_fd: invalid input.");
	}

	//u32(ctx->start_va);
	//u16(size_in_memory); 16-byte units
	//u16(size_in_file); 16-byte units
	//u16(ctx->ncalls.count);
	//u16(0xFFFF);
}
