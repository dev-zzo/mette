#ifndef __mette_vm_misc_h_included
#define __mette_vm_misc_h_included

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

static inline uint16_t vm_fetch16_ua(const void *addr)
{
	const uint8_t *ptr = addr;
#ifdef TARGET_IS_BE
	return ptr[0] | (ptr[1] << 8);
#else
	return ptr[1] | (ptr[0] << 8);
#endif
}

static inline uint32_t vm_fetch32_ua(const void *addr)
{
	const uint8_t *ptr = addr;
#ifdef TARGET_IS_BE
	return ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
#else
	return ptr[3] | (ptr[2] << 8) | (ptr[1] << 16) | (ptr[0] << 24);
#endif
}

static inline uint16_t vm_bswap16(uint16_t v)
{
	return ((v & 0xff00U) >> 8) | ((v & 0x00ffU) << 8);
}

static inline uint32_t vm_bswap32(uint32_t v)
{
	return (((v & 0xff000000U) >> 24) | ((v & 0x00ff0000U) >> 8) | ((v & 0x0000ff00U) << 8) | ((v & 0x000000ffU) << 24));
}

#ifdef TARGET_IS_BE
#define VM_LE16(x) (vm_bswap16(x))
#define VM_LE32(x) (vm_bswap32(x))
#else
#define VM_LE16(x) (x)
#define VM_LE32(x) (x)
#endif

#endif

