#ifndef __mette_vm_sysdeps_h_included
#define __mette_vm_sysdeps_h_included

/* Integration defines... */

#include <sys/types.h>
#include <stdint.h>

typedef  intptr_t vm_soperand_t;
typedef uintptr_t vm_uoperand_t;
typedef vm_uoperand_t vm_operand_t;

#if 0
#include "xmalloc.h"
#define vm_alloc xmalloc
#define vm_free xfree
#else
#include <malloc.h>
#define vm_alloc malloc
#define vm_free free
#endif

#include <unistd.h>
#include <fcntl.h>
#define vm_open open
#define vm_read read
#define vm_lseek lseek
#define vm_close close

#define __USE_MISC
#include <sys/mman.h>
#define vm_mmap mmap

#ifndef MAP_UNINITIALIZED
#define MAP_UNINITIALIZED (0)
#endif

#endif // __mette_vm_sysdeps_h_included

