#include "vm_loader.h"
#include "vm_sysdeps.h"
#include "vm_internal.h"
#include <elf.h>

#define EM_METTE 0x6969

#ifdef TARGET_IS_BE
#define ELF_LE16(x) (__builtin_bswap16(x))
#define ELF_LE32(x) (__builtin_bswap32(x))
#else
#define ELF_LE16(x) (x)
#define ELF_LE32(x) (x)
#endif

/*
The loader loads an ELF containing the byte code and executes it.
Note we cannot use the system's loader or go via the interpreter route
due to machines/endianness being different.
*/

static void vm_loader_process_load(vm_module_t *module, Elf32_Phdr *prog_hdr, int fd)
{
	int prot = 0;
	void *addr;
	
	if (ELF_LE32(prog_hdr->p_memsz) == 0) {
		return;
	}
	
	prot |= (ELF_LE32(prog_hdr->p_flags) & PF_R) ? PROT_READ : 0;
	prot |= (ELF_LE32(prog_hdr->p_flags) & PF_W) ? PROT_READ|PROT_WRITE : 0;
	
	addr = vm_mmap(
		(void *)ELF_LE32(prog_hdr->p_vaddr),
		ELF_LE32(prog_hdr->p_memsz),
		prot,
		MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED|MAP_POPULATE|MAP_UNINITIALIZED,
		-1,
		0);
	if (addr == MAP_FAILED) {
		vm_panic("vm_loader_process_load: failed to allocate memory.");
	}
	
	if (ELF_LE32(prog_hdr->p_filesz) > 0) {
		off_t old_pos = vm_lseek(fd, 0, SEEK_CUR);
		if (vm_read(fd, addr, ELF_LE32(prog_hdr->p_filesz)) != ELF_LE32(prog_hdr->p_filesz)) {
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
	
	if (ELF_LE16(elf_hdr.e_type) != ET_EXEC) {
		vm_panic("vm_load: cannot load non-executables.");
	}
	
	if (ELF_LE16(elf_hdr.e_machine) != EM_METTE) {
		vm_panic("vm_load: wrong machine.");
	}
	
	elf_length = vm_lseek(fd, 0, SEEK_END);
	if (ELF_LE32(elf_hdr.e_phoff) >= elf_length) {
		vm_panic("vm_load: invalid program headers offset.");
	}
	
	/* Read and process each segment. */
	vm_lseek(fd, ELF_LE32(elf_hdr.e_phoff), SEEK_SET);
	for (int hdr_index = 0; hdr_index < ELF_LE16(elf_hdr.e_phnum); ++hdr_index) {
		Elf32_Phdr prog_hdr;
		
		if (vm_read(fd, &prog_hdr, sizeof(prog_hdr)) != sizeof(prog_hdr)) {
			vm_panic("vm_load: cannot read a program header.");
		}
		
		/* OPT: may disregard this check. */
		if (sizeof(prog_hdr) < ELF_LE16(elf_hdr.e_phentsize)) {
			vm_lseek(fd, ELF_LE16(elf_hdr.e_phentsize) - sizeof(prog_hdr), SEEK_CUR);
		}
		
		switch (ELF_LE32(prog_hdr.p_type)) {
			case PT_LOAD:
				vm_loader_process_load(module, &prog_hdr, fd);
				break;
			default:
				/* Ignore unknown segments. */
				break;
		}
	}
	
	module->entry = (void *)ELF_LE32(elf_hdr.e_entry);
	return module;
}


