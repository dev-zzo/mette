#include "vm_loader.h"
#include "vm_sysdeps.h"
#include "vm_internal.h"
#include "vm_misc.h"
#include "vm_thunks.h"

#define DEBUG_PRINTS
#include "rtl_debug.h"

#define MOAR_CHECKS 1

vm_module_t *vm_load_fd(int fd)
{
	vm_module_t *module;
	size_t bytes_read;
	char signature[4];
	unsigned module_memsize;
	unsigned module_filesize;
	unsigned ep_offset;
	unsigned short ncalls_count;

	vm_lseek(fd, 0, SEEK_SET);

	DBGPRINT("vm_load_fd: checking signature.\n");
	if (vm_read(fd, signature, 4) != 4) {
		vm_panic("vm_load_fd: input too short.");
	}
	if (signature[0] != 'B' || signature[1] != 'A' || signature[2] != 'R' || signature[3] != 'F') {
		vm_panic("vm_load_fd: invalid input.");
	}

	bytes_read = vm_read(fd, &module_memsize, 4);
#if MOAR_CHECKS
	if (bytes_read != 4) {
		vm_panic("vm_load_fd: failed reading the data.");
	}
#endif

	bytes_read = vm_read(fd, &module_filesize, 4);
#if MOAR_CHECKS
	if (bytes_read != 4) {
		vm_panic("vm_load_fd: failed reading the data.");
	}
#endif

	bytes_read = vm_read(fd, &ep_offset, 4);
#if MOAR_CHECKS
	if (bytes_read != 4) {
		vm_panic("vm_load_fd: failed reading the data.");
	}
#endif

	bytes_read = vm_read(fd, &ncalls_count, 2);
#if MOAR_CHECKS
	if (bytes_read != 2) {
		vm_panic("vm_load_fd: failed reading the data.");
	}
#endif

	vm_lseek(fd, 0x20, SEEK_SET); /* Just filling for now */

#if TARGET_IS_BE
	module_memsize = vm_bswap32(module_memsize);
	module_filesize = vm_bswap32(module_filesize);
	ep_offset = vm_bswap32(ep_offset);
	ncalls_count = vm_bswap16(ncalls_count);
#endif

	DBGPRINT("vm_load_fd: image size in memory: %08x\n", module_memsize);
	DBGPRINT("vm_load_fd: image size in file: %08x\n", module_filesize);
	DBGPRINT("vm_load_fd: native calls count: %d\n", ncalls_count);

	module = (vm_module_t *)vm_alloc(sizeof(*module));
	if (!module) {
		vm_panic("vm_load_fd: failed to alloc vm_module struct.");
	}

	module->next = NULL;
	module->base = vm_alloc(module_memsize);
	if (!module->base) {
		vm_panic("vm_load_fd: failed to alloc module memory");
	}
	module->entry = module->base + ep_offset;

	bytes_read = vm_read(fd, module->base, module_filesize);
	if (bytes_read != module_filesize) {
		vm_panic("vm_load_fd: failed to read module body: read %d, expected %d", bytes_read, module_filesize);
	}

	if (ncalls_count) {
		int index = 0;
		size_t ncalls_size = ncalls_count * sizeof(void *);

		module->ncalls_table = vm_alloc(ncalls_size);
		if (!module->ncalls_table) {
			vm_panic("vm_load_fd: failed to alloc ncalls table memory");			
		}
		if (vm_read(fd, module->ncalls_table, ncalls_size) != ncalls_size) {
			vm_panic("vm_load_fd: failed to read ncalls table: read %d, expected %d", bytes_read, module_filesize);
		}
		DBGPRINT("vm_load_fd: fixing up ncalls table.\n");
		while (index < ncalls_count) {
			uint32_t hash;
			vm_thunk_t thunk;

			hash = (uint32_t)module->ncalls_table[index];
#if TARGET_IS_BE
			hash = vm_bswap32(hash);
#endif
			DBGPRINT("vm_load_fd: looking up %08x... ", hash);
			thunk = vm_lookup_thunk(hash);
			if (!thunk) {
				vm_panic("vm_load_fd: failed to resolve thunk %08x", hash);
			}
			DBGPRINT("@%08x.\n", thunk);
			module->ncalls_table[index] = thunk;
			++index;
		}
	} else {
		module->ncalls_table = NULL;
	}

	DBGPRINT("vm_load_fd: done.\n");
	return module;
}
