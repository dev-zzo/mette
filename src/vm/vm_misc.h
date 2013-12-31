#ifndef __mette_vm_misc_h_included
#define __mette_vm_misc_h_included

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifdef TARGET_IS_BE
#define VM_LE16(x) (__builtin_bswap16(x))
#define VM_LE32(x) (__builtin_bswap32(x))
#else
#define VM_LE16(x) (x)
#define VM_LE32(x) (x)
#endif

#define FASTCALL __attribute__((fastcall))

#endif

