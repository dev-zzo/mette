#ifndef __mette_vm_sysdeps_h_included
#define __mette_vm_sysdeps_h_included

/* Integration defines... */

#include <sys/types.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

typedef  intptr_t vm_soperand_t;
typedef uintptr_t vm_uoperand_t;
typedef vm_uoperand_t vm_operand_t;

#if 1
#include "rtl_memory.h"
#include "syscalls.h"
#define vm_open sys_open
#define vm_read sys_read
#define vm_lseek sys_lseek
#define vm_close sys_close
#define vm_alloc rtl_alloc
#define vm_free rtl_free
#else
#include <malloc.h>
#define vm_open open
#define vm_read read
#define vm_lseek lseek
#define vm_close close
#define vm_alloc malloc
#define vm_free free
#endif


#define __USE_MISC
#include <sys/mman.h>
#define vm_mmap mmap

#ifndef MAP_UNINITIALIZED
#define MAP_UNINITIALIZED (0)
#endif

#endif // __mette_vm_sysdeps_h_included

