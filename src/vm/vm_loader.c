#include "vm_loader.h"

vm_section_t *vm_load_fd(vm_context_t *ctx, int fd)
{
	char shabang[2];
	
	if (vm_read(fd, shabang, 2) != 2) {
		vm_panic("vm_load: invalid input file.");
	}
	
	/* Skip the shabang string */
	if (shabang[0] == '#' && shabang[1] == '!') {
		while (shabang[0] != '\n') {
			if (vm_read(fd, shabang, 1) != 1) {
				vm_panic("vm_load: invalid input file.");
			}
		}
	}
	
	/* TODO: decrypt */
	/* TODO: verify signature */
	
	
	return 0;
}


