#include "vm_loader.h"
#include "vm_sysdeps.h"
#include "vm_internal.h"
#include "vm_misc.h"

static void vm_resolve_ncalls()
{

}

vm_module_t *vm_load_fd(int fd)
{
	vm_module_t *module;
	char signature[4];
	unsigned module_memsize;
	unsigned module_filesize;
	unsigned ep_offset;

	if (vm_read(fd, signature, 4) != 4) {
		vm_panic("vm_load_fd: input too short.");
	}
	if (signature[0] != 'B' || signature[1] != 'A' || signature[2] != 'R' || signature[3] != 'F') {
		vm_panic("vm_load_fd: invalid input.");
	}

	vm_read(fd, &module_memsize, 4);
	vm_read(fd, &module_filesize, 4);
	vm_read(fd, &ep_offset, 4);
#ifdef TARGET_IS_BE
	module_memsize = vm_bswap32(module_memsize);
	module_filesize = vm_bswap32(module_filesize);
	ep_offset = vm_bswap32(ep_offset);
#endif

}
