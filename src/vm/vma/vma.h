#ifndef __mette_vm_asm_h_included
#define __mette_vm_asm_h_included

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef uint32_t vma_vaddr_t;
typedef struct _vma_symbol_t vma_symbol_t;
typedef struct _vma_symtab_t vma_symtab_t;
typedef struct _vma_symref_t vma_symref_t;
typedef struct _vma_expr_t vma_expr_t;
typedef struct _vma_expr_list_t vma_expr_list_t;
typedef struct _vma_insn_t vma_insn_t;
typedef struct _vma_context_t vma_context_t;
/*
 * Symbols
 */

struct _vma_symbol_t {
	vma_symbol_t *next;
	const char *name;
	union {
		unsigned id;
		vma_insn_t *location;
		vma_expr_t *value;
	} u;
};

struct _vma_symtab_t {
	vma_symbol_t *head;
	vma_symbol_t *tail;
	unsigned count;
};

extern void vma_symtab_init(vma_symtab_t *symtab);
extern vma_symbol_t *vma_symtab_define(vma_symtab_t *symtab, const char *name, int unique);
extern vma_symbol_t *vma_symtab_lookup(const vma_symtab_t *symtab, const char *name);

struct _vma_symref_t {
	union {
		const char *name;
		vma_symbol_t *sym;
	} u;
};

extern void vma_symref_init(vma_symref_t *ref, const char *name);
extern int vma_symref_resolve(vma_symref_t *ref, vma_symtab_t *symtab);

/*
 * Expressions
 */

typedef enum _vma_expr_type_t vma_expr_type_t;
enum _vma_expr_type_t {
	EXPR_LITERAL,
	EXPR_SYMREF,
	EXPR_OR,
	EXPR_AND,
	EXPR_XOR,
	EXPR_ADD,
	EXPR_SUB,
	EXPR_MUL,
	EXPR_DIV,
	EXPR_NEG,
	EXPR_NOT,
};

struct _vma_expr_t {
	vma_expr_t *next; /* in expr_list */
	vma_expr_type_t type;
	uint32_t value;
	union {
		vma_expr_t *child[2]; /* lhs, rhs */
		vma_symref_t symref;
	} u;
};

extern vma_expr_t *vma_expr_build_literal(int value);
extern vma_expr_t *vma_expr_build_symref(const char *name);
extern vma_expr_t *vma_expr_build_parent(vma_expr_type_t type, vma_expr_t *a, vma_expr_t *b);
extern uint32_t vma_expr_evaluate(vma_expr_t *expr, vma_context_t *ctx);

struct _vma_expr_list_t {
	vma_expr_t *head;
	vma_expr_t *tail;
	unsigned count;
};

extern vma_expr_list_t *vma_expr_list_create(void);
extern vma_expr_list_t *vma_expr_list_append(vma_expr_list_t *list, vma_expr_t *node);

/*
 * Instructions
 */

typedef enum _vma_insn_type_t vma_insn_type_t;
enum _vma_insn_type_t {
	INSN_ADD,
	INSN_SUB,
	INSN_MULU,
	INSN_MULS,
	INSN_DIVU,
	INSN_DIVS,
	INSN_AND,
	INSN_OR,
	INSN_XOR,
	INSN_NOT,
	INSN_LSL,
	INSN_LSR,
	INSN_ASR,
	INSN_CMP_LT,
	INSN_CMP_GT,
	INSN_CMP_B,
	INSN_CMP_A,
	INSN_CMP_EQ,
	INSN_LDC_0,
	INSN_LDC_1,
	INSN_LDC_2,
	INSN_LDC_8_U,
	INSN_LDC_8_S,
	INSN_LDC_32,
	INSN_LEA,
	INSN_LDM_8_U,
	INSN_LDM_8_S,
	INSN_LDM_16_U,
	INSN_LDM_16_S,
	INSN_LDM_32,
	INSN_STM_8,
	INSN_STM_16,
	INSN_STM_32,
	INSN_LOCALS,
	INSN_LDLOC,
	INSN_STLOC,
	INSN_DUP,
	INSN_SWAP,
	INSN_POP,
	INSN_BR,
	INSN_BR_T,
	INSN_BR_F,
	INSN_CALL,
	INSN_RET,
	INSN_ICALL,
	INSN_IJMP,
	INSN_NCALL,

	INSN_ASM_KEYWORDS,

	INSN_DEFB,
	INSN_DEFH,
	INSN_DEFW,
	INSN_DEFS,
	INSN_RESB,
	INSN_RESH,
	INSN_RESW,

	INSN_MAX,
};

struct _vma_insn_t {
	vma_insn_t *next;
	vma_insn_type_t type;
	vma_vaddr_t start_addr;
	union {
		vma_symref_t symref;
		vma_expr_t *expr;
		vma_expr_list_t *expr_list;
		struct {
			unsigned length;
			const char *buffer;
		} text;
	} u;
	struct {
		unsigned allocated : 1;
	} flags;
	int line;
};

extern vma_insn_t *vma_insn_build(vma_insn_type_t type);
extern void vma_output_insn(vma_insn_t *node);

/*
 * Parsing
 */

struct _vma_context_t {
	const char *input_name;
	FILE *input;
	FILE *output;
	const char *start_symbol;
	vma_symtab_t labels;
	vma_symtab_t constants;
	vma_symtab_t ncalls;
	vma_insn_t *insns_head;
	vma_insn_t *insns_tail;
	vma_vaddr_t start_va;
	vma_vaddr_t bss_va;
	vma_vaddr_t end_va;
};

extern void vma_context_init(vma_context_t *ctx);
extern void vma_assemble(vma_context_t *ctx);

/*
 * Output generation
 */

extern void vma_generate(vma_context_t *ctx);
extern void vma_output_u8(uint8_t value);
extern void vma_output_u16(uint16_t value);
extern void vma_output_u32(uint32_t value);

/* 
 * Helper functions
 */

#define VMA_ALIGN(x,a) (((x) + (a) - 1) & ~((a) - 1))

extern void *vma_malloc(size_t count);
extern void vma_free(void *ptr);

#define VMA_ASSERT(cond) (cond) ? (void)0 : vma_abort("%s:%d: assertion failed: %s", __FILE__, __LINE__, #cond)

extern void vma_error(const char *format, ...);
extern void vma_abort(const char *format, ...);
extern void vma_abort_on_errors(void);
extern void vma_debug_print(const char *format, ...);

extern int vma_errors;
extern int vma_debug;

#endif // __mette_vm_asm_h_included
