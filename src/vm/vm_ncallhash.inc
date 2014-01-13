#ifndef __mette_vm_ncallhash_h_included
#define __mette_vm_ncallhash_h_included

static uint32_t vma_hash_name(const char *name)
{
	uint32_t hash = 0;
	const char *ptr = name;

	while (*ptr) {
		hash = (hash * 61 + *ptr) ^ (hash >> 16);
		ptr++;
	}

	return hash;
}

#endif // __mette_vm_ncallhash_h_included
