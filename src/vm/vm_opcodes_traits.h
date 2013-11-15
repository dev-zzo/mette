#ifndef __vm_opcodes_traits_h_included
#define __vm_opcodes_traits_h_included

typedef struct {
	unsigned char pop_count : 2;
	unsigned char push_count : 2;
} vm_opcode_traits_t;

#endif // __vm_opcodes_traits_h_included

