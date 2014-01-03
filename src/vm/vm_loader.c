#include "vm_loader.h"
#include "vm_sysdeps.h"
#include "vm_internal.h"
#include <elf.h>

#define EM_METTE 0x6969

/*
The loader loads an ELF containing the byte code and executes it.
Note we cannot use the system's loader or go via the interpreter route
due to machines/endianness being different.
*/

static void vm_loader_process_load(vm_module_t *module, Elf32_Phdr *prog_hdr, int fd)
{
	int prot = 0;
	void *addr;
	
	if (prog_hdr->p_memsz == 0) {
		return;
	}
	
	prot |= (VM_LE32(prog_hdr->p_flags) & PF_R) ? PROT_READ : 0;
	prot |= (VM_LE32(prog_hdr->p_flags) & PF_W) ? PROT_READ|PROT_WRITE : 0;
	
	/* Kernels that we care about support MAP_ANONYMOUS. */
	addr = vm_mmap(
		(void *)VM_LE32(prog_hdr->p_vaddr),
		VM_LE32(prog_hdr->p_memsz),
		prot,
		MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED|MAP_POPULATE,
		-1,
		0);
	if (addr == MAP_FAILED) {
		vm_panic("vm_loader_process_load: failed to allocate memory.");
	}
	
	if (VM_LE32(prog_hdr->p_filesz) > 0) {
		off_t old_pos = vm_lseek(fd, 0, SEEK_CUR);
		if (vm_read(fd, addr, VM_LE32(prog_hdr->p_filesz)) != VM_LE32(prog_hdr->p_filesz)) {
			vm_panic("vm_loader_process_load: failed to read segment content.");
		}
		vm_lseek(fd, old_pos, SEEK_SET);
	}
}

vm_module_t *vm_load_fd(int fd)
{
	vm_module_t *module = (vm_module_t *)vm_alloc(sizeof(vm_module_t));
	Elf32_Ehdr elf_hdr;
	off_t elf_length;
	
	if (!module) {
		return module;
	}
	
	/* Read and validate the ELF header. */
	
	if (vm_read(fd, &elf_hdr, sizeof(elf_hdr)) != sizeof(elf_hdr)) {
		vm_panic("vm_load: not a valid ELF file.");
	}
	
	if (elf_hdr.e_ident[EI_MAG0] != ELFMAG0 
		|| elf_hdr.e_ident[EI_MAG1] != ELFMAG1
		|| elf_hdr.e_ident[EI_MAG2] != ELFMAG2
		|| elf_hdr.e_ident[EI_MAG3] != ELFMAG3
		|| elf_hdr.e_ident[EI_CLASS] != ELFCLASS32
		|| elf_hdr.e_ident[EI_DATA] != ELFDATA2LSB) {
		vm_panic("vm_load: not a valid ELF file.");
	}
	
	if (VM_LE16(elf_hdr.e_type) != ET_EXEC) {
		vm_panic("vm_load: cannot load non-executables.");
	}
	
	if (VM_LE16(elf_hdr.e_machine) != EM_METTE) {
		vm_panic("vm_load: wrong machine.");
	}
	
	elf_length = vm_lseek(fd, 0, SEEK_END);
	if (VM_LE32(elf_hdr.e_phoff) >= elf_length) {
		vm_panic("vm_load: invalid program headers offset.");
	}
	
	/* Read and process each segment. */
	vm_lseek(fd, VM_LE32(elf_hdr.e_phoff), SEEK_SET);
	for (int hdr_index = 0; hdr_index < VM_LE16(elf_hdr.e_phnum); ++hdr_index) {
		Elf32_Phdr prog_hdr;
		
		if (vm_read(fd, &prog_hdr, sizeof(prog_hdr)) != sizeof(prog_hdr)) {
			vm_panic("vm_load: cannot read a program header.");
		}
		
		/*
		if (sizeof(prog_hdr) < VM_LE16(elf_hdr.e_phentsize)) {
			vm_lseek(fd, VM_LE16(elf_hdr.e_phentsize) - sizeof(prog_hdr), SEEK_CUR);
		}
		*/
		
		switch (VM_LE32(prog_hdr.p_type)) {
			case PT_LOAD:
				vm_loader_process_load(module, &prog_hdr, fd);
				break;
			default:
				/* Ignore unknown segments. */
				break;
		}
	}
	
	module->next = NULL;
	module->entry = (void *)VM_LE32(elf_hdr.e_entry);
	return module;
}


